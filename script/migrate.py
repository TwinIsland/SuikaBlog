import sqlite3
import re

conn = sqlite3.connect('suika.db')
c = conn.cursor()

def replace_urls_for_upload(content):
    # Remove <!--markdown--> prefix if it exists
    content = re.sub(r'^<!--markdown-->', '', content, flags=re.MULTILINE)
    # Replace URLs
    content = re.sub(r'https://cirno.me/', '/', content, flags=re.MULTILINE)
    url_pattern = r'https?://cirno\.me/usr/(uploads/[\w./-]+\.\w+)(?:#.*|\s|$)'
    content = re.sub(url_pattern, r'\1', content)
    # Remove fragments like #vwid=... from all URLs
    fragment_pattern = r'#vwid=\d+&vhei=\d+'
    content = re.sub(fragment_pattern, '', content)

    content = re.sub(r'/usr(/uploads/\d{4}/\d{2}/\d{2}/[\w.-]+\.\w+)(?:#.*|\s|$)', r'\1', content)


    return content


conn.create_function("REGEX_REPLACE_UPLOAD", 1, replace_urls_for_upload)

c.execute("INSERT INTO Meta (Name, Type) VALUES ('selected', 'tag')")
c.execute("INSERT INTO PostMeta (PostID, MetaName) VALUES ('165', 'selected')")
c.execute("INSERT INTO PostMeta (PostID, MetaName) VALUES ('166', 'selected')")
c.execute("INSERT INTO PostMeta (PostID, MetaName) VALUES ('171', 'selected')")

c.execute("")
conn.commit()
conn.close()
