#!/usr/bin/python
import json
from bson import json_util
import bottle
from bottle import route, run, get, request, abort
from pymongo import MongoClient

connection = MongoClient('localhost', 27017)
result = 0
db = connection['market']
collection = db['stocks']


# CREATE: Creates a new stock in the StocksDB
# TEST COMMAND: curl -H "Content-Type: application/json" -X POST -d @mock_data.json http://localhost:8080/createStock
@route('/createStock', method='post')
def create_stock():
    buisness = request.json
    try:
        result = collection.save(buisness)
        return gen_message("DOCUMENT INSERTED\n")
    except Exception as e:
        print gen_message("ERROR: Invalid query.")
        abort(400, e)


# UPDATE: Updates documents volume by Ticker value.
# TEST COMMAND: curl http://localhost:8080/AA
@route('/updateStock/<ticker>', method='put')
def update_stock(ticker):

    volume = request.forms['volume']

    if (volume < 0):
        print gen_message('ERROR: Volume needs to be greater than 0.\n')
        abort(400, Exception)

    search = json.loads("{ \"Ticker\" : \"" + ticker + "\" }")
    update = json.loads(
        "{ \"$set\" : { \"Volume\" : \"" + str(volume) + "\" } }")
    query = {}

    try:
        return collection.update(search, update)
    except Exception as e:
        abort(400, str(e))


# DELETE: Deletes an entry by Ticker
# curl http://localhost:8080/deleteStock/AA
@route('/deleteStock/<ticker>', method='get')
def delete_stock(ticker):
    ticker = json.loads("{ \"Ticker\" : \"" + ticker + "\" }")
    try:
        result = collection.remove(ticker)
    except Exception as e:
        abort(400, str(e))
    return result


# READ: Finds documents whose "50-Day Simple Moving Average" is between two ranges.
# TEST COMMAND: curl "http://localhost:8080/findStocks?low="0"&high="1""
@route('/findStocks', method='get')
def find_stock():
    try:
        low = request.query.low
        high = request.query.high
        query = json.loads(
            "{\"50-Day Simple Moving Average\" : { \"$gte\": " + low + ", \"$lt\": " + high + " }}")
        return gen_message(str(collection.find(query).count()) + " | Results were found between " + low + " & " + high + "\n")
    except Exception as e:
        print gen_message("ERROR: Failed query.\n")
        abort(400, e)


# READ: Finds documents whose "50-Day Simple Moving Average" is between two ranges.
# TEST COMMAND: curl http://localhost:8080/findStocksIndustry/Medical%20Laboratories%20%26%20Research
@route('/findStocksIndustry/<industry>', method='get')
def find_stock_industry(industry):
    try:
        query = json.loads("{\"Industry\": \"" + industry + "\" }")
        result = collection.find(query)
    except Exception as e:
        print gen_message("ERROR: Failed query.\n")
        abort(400, e)
    msg = ""
    i = 0
    for info in result:
        if (i == 5):
            msg = msg + info['Ticker'] + ", \n"
            i = 0
        else:
            msg = msg + info['Ticker'] + ", "
            i = i + 1
    return gen_message(msg + "\n\n~ Tickers from Industry | " + request.query.industry + " ~\n")


# AGGREGATE: Aggregates "Stocks Outstanding" by sector & industry.
# TEST COMMAND: curl http://localhost:8080/findOSBS?sector="Healthcare"
@route('/findOSBS', method='get')
def find_stock_osbs():
    try:
        query = "[{ \"$match\": { \"Sector\": \"" + request.query.sector + \
            "\" } }, { \"$group\": { \"_id\": \"$Industry\", \"total\": { \"$sum\": \"$Shares Outstanding\" } } }]"
        query = json.loads(query)
        result = collection.aggregate(query)
    except Exception as e:
        print gen_message("ERROR: Failed query.\n")
        abort(400, e)
    msg = ""
    for info in result:
        msg = msg + str(info['_id']) + " | " + str(info['total']) + '\n'
    return gen_message("\nIndustry | Stocks Outstanding\n\n" + msg + "\n")


# FIND: Find the top five stocks of an industry.
# TEST COMMAND: curl http://localhost:8080/industryReport/Medical%20Laboratories%20%26%20Research
@route('/industryReport/<industry>', method='get')
def find_stock_industry_top_five(industry):
    try:
        query = json.loads("{ \"Industry\" : \"" + industry + "\" }")
        result = collection.find(query).sort("price", 1).limit(5)
    except Exception as e:
        print gen_message("ERROR: Invalid query.\n")
        abort(400, e)
    msg = ""
    for info in result:
        msg = msg + str(info['Company']) + " | $" + str(info['Price']) + '\n'
    return gen_message("\n" + msg + "\n")


# FIND: Finds information for relevant tickers.FIND
# TEST COMMAND: curl -H "Content-Type: application/json" -X POST -d @tickers_mock_list.json http://localhost:8080/stocksPortfolio
@route('/stocksPortfolio', method='post')
def find_stock_tickers():
    msg = ""
    try:
        tickers = request.json
        for ticker in tickers:
            query = json.loads("[{\"$match\":{\"Ticker\": \"" + ticker +
                               "\"}},{\"$group\":{\"_id\":\"$Ticker\",\"total_one\":{\"$sum\":\"$Shares Outstanding\"},\"total_two\":{\"$sum\":\"$Price\"}, \"total_three\":{\"$sum\":\"$Volume\"}}}]")
            result = collection.aggregate(query)
            for info in result:
                msg = msg + "Information for " + ticker + " Ticker | Total Shares Outstanding: " + \
                    str(info['total_one']) + " | Total Price: " + str(info["total_two"]
                                                                      ) + " | Total Volume: " + str(info["total_three"]) + "\n"

    except Exception as e:
        print gen_message("ERROR: Invalid query.\n")
        abort(400, e)
    return gen_message("\n" + msg + "\n")

# basic functions go here


def gen_message(string):
    stars = "*" * 50+"\n"
    return stars+string+stars


if __name__ == '__main__':
    run(host='localhost', port=8080)
