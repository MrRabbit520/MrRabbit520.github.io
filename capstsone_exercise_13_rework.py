#Exercise 13: Making Change
#(Solved—33 Lines)
#Consider the software that runs on a self-checkout machine. One task that it must be
#able to perform is to determine how much change to provide when the shopper pays
#for a purchase with cash.
#Write a program that begins by reading a number of cents from the user as an
#integer. Then your program should compute and display the denominations of the
#coins that should be used to give that amount of change to the shopper. The change
#should be given using as few coins as possible. Assume that the machine is loaded
#with pennies, nickels, dimes, quarters, loonies and toonies.
#A one dollar coin was introduced in Canada in 1987. It is referred to as a
#loonie because one side of the coin has a loon (a type of bird) on it. The two
#dollar coin, referred to as a toonie, was introduced 9 years later. It’s name is
#derived from the combination of the number two and the name of the loonie.

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