### A Whole Bunch of CRUD
[MongoDB database work.](https://github.com/RealMrRabbit/realmrrabbit.github.io/blob/main/final_project_server.py)

### My Ventures Into OpenGL
[My basic OpenGL graphics engine.](www.google.com)

### Where I Started

```python
#User inputs
total_of_purchase = input("What was your bill's total? $")
amount_paid = input("How much money did you give? $")

#Main change_due
change_due = float(total_of_purchase) - float(amount_paid)

#Value of coins/Collection of change_due_type
values_dic = {
  'toonie' : 2,
  'loonie' : 1,
  'quarter' : 0.25,
  'dime' : 0.10,
  'nickle' : 0.05,
  'pennie' : 0.01,
}
change_due_type = {
  "toonies_due" : 0,
  "loonies_due" : 0,
  "quarters_due" : 0,
  "dimes_due" : 0,
  "nickles_due" : 0,
  "pennies_due" : 0,
}

#ERROR messages
not_enough = ('Looks like you have not given me enough money, you still owe $%s.') % (change_due)
exact = 'Looks like you are all set!'
error = 'looks like something went wrong please try again.'

#Puts values of change_data into a dictionary format
def into_dic(x):
    global change_due_type
    p = 0
    for y in change_due_type:
        if x[p] == 0:
            p += 1
            continue
        if x[p] != 0:
            change_due_type[y] += x[p]
            p += 1
            continue
    return change_due_type

#Allows for you to loop through on value in a for loop
def change(x):
    global change_due
    a = 0
    start = True
    while start:
        change_due += x
        round(change_due, 1)
        if change_due == 0:
            a += 1
            start = False
        if change_due < 0:
            a += 1
        if change_due > 0:
            change_due -= x
            round(change_due, 1)
            start = False
    round(change_due, 2)
    return a

#Main interface
def change_calc_dic():
    global change_due
    change_data = [0,0,0,0,0,0]
    #p=position in change_data
    p = 0
    if change_due > 0:
        return not_enough
    elif change_due == 0:
        return exact
    elif change_due < 0:
        for x in values_dic:
            a = change(values_dic[x])
            if a == 0:
                p += 1
                continue
            if a != 0:
                change_data[p] += a
                p += 1
                continue
    else:
        return error
    return into_dic(change_data)

print(change_calc_dic())
```

### Where I Am

```python
#Coin Values
values_dic = {
    'toonie': 2,
    'loonie': 1,
    'quarter': 0.25,
    'dime': 0.10,
    'nickle': 0.05,
    'pennie': 0.01,
}

# Simple function that goes through the coin dictionary counting back the change.
def count_change(change_due):
    print("We owe you the following change...")
    for x, y in values_dic.items():
        if change_due == 0.0:
            break
        floor = change_due // float(y)
        change_due -= round(y * floor, 2)
        change_due = round(change_due, 2)
        print(x + ": " + str(int(floor)))

def main():
    total_paid = input("What was your bill's total? $")
    amount_paid = input("How much money did you give? $")
    change_due = float(amount_paid) - float(total_paid)
    if change_due < 0:
        return ('Looks like you have not given me enough money, you still owe $%s.') % (change_due)
    elif change_due == 0:
        return 'Looks like you are all set!'
    elif change_due > 0:
        count_change(change_due)
    else:
        return 'looks like something went wrong please try again.'

main()
```

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.

[editor on GitHub](https://github.com/realmrrabbit/realmrrabbit.github.io/edit/main/index.md)
