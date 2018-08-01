import datetime

class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def intro(self):
        msg = "%s: %s (%d)" % (datetime.datetime.now(), self.name, self.age)
        return msg

def mymul(num1, num2):
    return num1 * num2
    
def mydouble(num):
    return num * 2
    
intVar = 1048576
stringVar = 'Hello, world'
listVar = ['rocky', 505, 2.23, 'wei', 70.2]
dictVar = {'Name': 'Rocky', 'Age': 17, 'Class': 'Top'}