import sqlite3
import cv2

#suporte a sqlite3

#insere imagem como BLOB e data como varchar(30)
def insert(date, Image):
    conn = sqlite3.connect('cparking.db')
    c = conn.cursor()
    c.execute("INSERT INTO car (date, image) values (?, ?)", (date, Image))
    conn.commit()
    print('commit')
    conn.close()