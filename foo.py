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
floatVar = 355.0/113.0
complexVar = 2+3j
stringVar = 'Hello, world'
bytesVar = bytes('中国🇨🇳加拿大🇨🇦', 'utf-8')
listVar = ['rocky', 505, 2.23, 'wei', 70.2]
tupleVar = ('juice', 'beer', 999, 'coke', 39.11, 'water', 'soda')
dictVar = {'Name': 'Rocky', 'Age': 17, 'Class': 'Top'}
rocky = Person('rocky', 21)
