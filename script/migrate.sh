rm suika.db
sqlite3 suika.db < migrate.sql
python3 migrate.py