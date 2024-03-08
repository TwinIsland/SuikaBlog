import sqlite3
import re

conn = sqlite3.connect('suika.db')
c = conn.cursor()

def replace_urls(content):
    pattern = r'https://cirno.me/usr/(uploads/[\w./-]+\.\w+)(?:#.*|\s|$)'
    return re.sub(pattern, r'\1', content)

def replace_urls_for_upload(content):
    pattern = r'https?://cirno\.me/usr/(uploads/[\w./-]+\.\w+)(?:#.*|\s|$)'
    return re.sub(pattern, r'\1', content)


conn.create_function("REGEX_REPLACE", 1, replace_urls)
conn.create_function("REGEX_REPLACE_UPLOAD", 1, replace_urls_for_upload)


c.execute("UPDATE POSTS SET CONTENT = REGEX_REPLACE(CONTENT)")
c.execute("UPDATE POSTS SET CONTENT = REGEX_REPLACE_UPLOAD(CONTENT)")

conn.commit()
conn.close()
