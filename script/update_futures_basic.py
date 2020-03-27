# coding=utf-8
 
import os
import time
import sqlite3  
import numpy as np  
import pandas as pd  
import tushare as ts  
import datetime as dt  
from pypinyin import pinyin, lazy_pinyin
  
ROOT_DIR = './' 
DB_FILE_PATH = '../build/Win32/Debug/ExchBase.kd'
#DB_FILE_PATH = './ExchBase.kd'
    
class STOCKBASIC:  
    def __init__(self):  
        self.cal_dates = ts.trade_cal() #return calendar of exchange center, DataFrame, calendarDate,isOpen  
        self.data_dir = "C:/"
        if "STK_DATA_DIR" in os.environ:
            self.data_dir = os.environ["STK_DATA_DIR"] 
        
        self.file_ok_ext = ".ok"    
        log_dir = self.data_dir + "\\log\\"
        if not os.path.exists(log_dir):
            os.makedirs(log_dir) 
        self.log_file_path = log_dir + "getklog_" + time.strftime("%Y%m%d", time.localtime()) + ".txt"   
        self.log_file_handle = open(self.log_file_path, 'a')
        self.g_db_conn = False
        self.g_db_conn = self.open_db()
        
    def __del__(self):
        print("del self")
        self.close_db()
        if self.log_file_handle:
            self.log_file_handle.close()
        
    def open_db(self):
        ''' open data base 保存数据库'''
        global DB_FILE_PATH 
        if not os.access(DB_FILE_PATH, os.F_OK):
            #print ("%s not exist\n" % DB_FILE_PATH)
            os._exit(0)
        self.g_db_conn = sqlite3.connect(DB_FILE_PATH, detect_types=sqlite3.PARSE_COLNAMES)
        if self.g_db_conn is not None:
            print ("opened {0} successful!".format(DB_FILE_PATH))
            pass
        else:
            #print ("opened database fail!")
            os._exit(0)
        self.g_db_conn.text_factory = lambda x : str(x, 'utf-8', 'ignore')
        return self.g_db_conn
        
    def close_db(self): 
        print("db commit")
        self.g_db_conn.commit()
        self.g_db_conn.close() 
    
    def getTargetKDataDir(self, code):
        target_path = self.data_dir + "/" + code + "/kline"
        #print("saveCodeTick2File : %s %s" %(code, target_path) )
        if not os.path.isdir(target_path):
            os.makedirs(target_path)
        return target_path
        
    def write_log(self, content):
        if self.log_file_handle:
            self.log_file_handle.write(content + "\n")
            self.log_file_handle.flush()
            
    def is_ascii(self, s):
        return all(ord(c) < 128 for c in s)
    
    def getpinyinhead(self, str): 
        str = str.replace(' ', '')
        head_arry = []
        for s in str:
            if not self.is_ascii(s) :
                tmp = lazy_pinyin(s) 
                #print(tmp[0][0]) 
                head_arry.append(lazy_pinyin(s)[0][0].upper())
                #print(s)
            else:
                head_arry.append(s.upper())   
        return head_arry
        
    def get_futures_baseinfo(self):
        count = 0
        self.cur = self.g_db_conn.cursor()
        for row in self.cur:
            count = row[0]
            break
        stock_info = ts.get_stock_basics() 
        self.cur.execute("SELECT count(code) FROM stock") 
        num = 0
        #type = 0; # 0--stock ; 1--index ; 2--futures
        #for i in stock_info.index:
        #    py_str = ''.join(self.getpinyinhead(stock_info.ix[i]['name'])) 
        #    sql = "INSERT OR REPLACE INTO stock VALUES(?, ?, ?, ?, ?, ?, ?, '')"
        #    self.cur.execute(sql, (i, type, stock_info.ix[i]['name'], py_str, str(stock_info.ix[i]['timeToMarket']), stock_info.ix[i]['industry'], stock_info.ix[i]['area']))
        #    #print("{}, {}, {}, {}, {}, {}, ''".format(i, stock_info.ix[i]['name'], py_str, str(stock_info.ix[i]['timeToMarket']), stock_info.ix[i]['industry'], stock_info.ix[i]['area']))
        #    num += 1
        self.cur.execute("DELETE FROM stock")
        type = 2; # 0--stock ; 1--index ; 2--futures
        index_array = [ ('SCL9', '原油指数', 'YYZS', 30)
                       ,('SC2007', '原油2007', 'YYELLQ', 30)
                       ,('SC2006', '原油2006', 'YYELLL', 30)
                       ,('SC2005', '原油2005', 'YYELLW', 30)
                       ,('AU2008', '黄金2008', 'HJELLB', 30)
                       ,('AU2006', '黄金2006', 'HJELLL', 30)
                       ,('AU2004', '黄金2004', 'HJELLS', 30)
                       ,('AG2007', '白银2007', 'BYELLQ', 30)
                       ,('AG2006', '白银2006', 'BYELLL', 30)
                       ,('RU2005', '橡胶2005', 'XJELLW', 30)
                       ,('I2005', '铁矿石2005', 'TKSELLW', 29)
                       ,('I2006', '铁矿石2006', 'TKSELLL', 29)
                       ,('I2007', '铁矿石2007', 'TKSELLQ', 29)
                       ,('Y2005', '豆油2005', 'DYELLW', 29)
                       ,('Y2004', '豆油2004', 'DYELLS', 29)
                       ,('JD2005', '鸡蛋2005', 'JDELLW', 29)
                       ,('JD2006', '鸡蛋2006', 'JDELLL', 29)
                       ,('CF2005', '郑棉2005', 'ZMELLW', 28)
                       ,('MA2005', '甲醇2005', 'JCELLW', 28)
                       ,('MA2006', '甲醇2006', 'JCELLL', 28)
                       ,('CJ1912', '红枣1912', 'HZYJYE', 28)
                       ,('FG2005', '玻璃2005', 'BLELLW', 28)
                       ,('IF1910', '股指期货1910', 'GZQHYJLQ', 47)
                       ,('IF300', '股指期货300', 'GZQHSLL', 47)]
        for i in range(0, len(index_array)):
            sql = "INSERT OR REPLACE INTO stock VALUES(?, ?, ?, ?, 0, '', '', '', ?)"
            self.cur.execute(sql, (index_array[i][0], type, index_array[i][1], index_array[i][2], index_array[i][3]))
            print("insert {} {} {} {} {}".format(index_array[i][0], type, index_array[i][1], index_array[i][2], index_array[i][3]))
            num += 1
            
        self.g_db_conn.commit()
        print("has insert or replace {} records".format(num))
        return "ok"
        
if __name__ == "__main__":  
    if "PYTHONPATH" in os.environ:
        mystr = os.environ["PYTHONPATH"] 
        print(mystr)
    code = "601699"
    obj = STOCKBASIC()
    if 1:
        obj.get_futures_baseinfo()
        
        
        
        