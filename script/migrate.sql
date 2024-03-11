PRAGMA foreign_keys = ON;

CREATE TABLE Posts (
    PostID INTEGER PRIMARY KEY AUTOINCREMENT,
    Title VARCHAR(255) NOT NULL,
    Banner VARCHAR(255),
    Excerpt VARCHAR(255),
    Content TEXT NOT NULL,
    IsPage INTEGER NOT NULL DEFAULT 0,  -- 0 or 1
    CreateDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    DateModified DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UpVoted INT NOT NULL DEFAULT 0,
    Views INT DEFAULT 0
);

CREATE TABLE Visitors (
    Name VARCHAR(64) PRIMARY KEY,
    Email TEXT,
    Website TEXT,
    Banned INTEGER,
    Ip VARCHAR(64) NOT NULL
);

CREATE TABLE Comment (
    CommentID INTEGER PRIMARY KEY AUTOINCREMENT,
    PostID INTEGER NOT NULL,
    AuthorName VARCHAR(64), 
    CreateDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    Content TEXT NOT NULL,
    UpVoted INTEGER DEFAULT 0,
    FOREIGN KEY (PostID) REFERENCES Posts(PostID) ON DELETE CASCADE,
    FOREIGN KEY (AuthorName) REFERENCES Visitors(Name) ON DELETE CASCADE
);

CREATE TABLE Activity (
    ActivityID INTEGER PRIMARY KEY AUTOINCREMENT,
    AuthorName INTEGER,
    Description TEXT NOT NULL,
    CreateDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (AuthorName) REFERENCES Visitors(Name)
);

CREATE TABLE Meta (
    Name VARCHAR(32) PRIMARY KEY,
    Type VARCHAR(32) NOT NULL  -- tag or category
);

CREATE TABLE PostMeta (
    PostID INTEGER,
    MetaName VARCHAR(32),
    FOREIGN KEY (PostID) REFERENCES Posts(PostID) ON DELETE CASCADE,
    FOREIGN KEY (MetaName) REFERENCES Meta(Name) ON DELETE CASCADE,
    PRIMARY KEY (PostID, MetaName)
); 

-- The field that allowed operation by plugins
CREATE TABLE Info (
    Key VARCHAR(64) PRIMARY KEY,
    Value VARCHAR(128)
);

-- Triggers to update Activity when detect new comment
CREATE TRIGGER InsertCommentActivity AFTER INSERT ON Comment
BEGIN
    INSERT INTO Activity (AuthorName, description, createDate)
    SELECT NEW.AuthorName, 'comment on: ' || Posts.Title, NEW.createDate
    FROM Posts
    WHERE Posts.PostID = NEW.postID;
END;


-- Trigger to check 'Post' insert and update
CREATE TRIGGER CheckIsPageBeforeInsert
BEFORE INSERT ON Posts
FOR EACH ROW
BEGIN
    SELECT CASE
        WHEN NEW.IsPage NOT IN (0, 1) THEN
            RAISE(FAIL, 'IsPage must be either 0 or 1.')
        END;
END;

-- Trigger to check 'Meta' insert and update
CREATE TRIGGER CheckMetaTypeBeforeInsert
BEFORE INSERT ON Meta
FOR EACH ROW
BEGIN
    SELECT CASE
        WHEN NEW.type NOT IN ('tag', 'category') THEN
            RAISE(FAIL, 'MetaType must be either tag or category.')
        END;
END;

-- attaching the databases
ATTACH DATABASE 'source.db' AS source;
ATTACH DATABASE 'suika.db' AS suika;

-- Inserting posts, filtering out pages and titles with periods
INSERT INTO suika.Posts (PostID, Title, Content, createDate, DateModified, UpVoted, Views)
SELECT cid, title, text, datetime(created, 'unixepoch'), datetime(modified, 'unixepoch'), likes, viewsNum
FROM source.blog_contents
WHERE type != 'page' 
AND title NOT LIKE '%.%';

INSERT INTO suika.Posts (PostID, Title, Content, createDate, DateModified, UpVoted, Views, IsPage)
SELECT cid, title, text, datetime(created, 'unixepoch'), datetime(modified, 'unixepoch'), likes, viewsNum, 1
FROM source.blog_contents
WHERE type = 'page' 
AND title NOT LIKE '%.%';


-- Updating Excerpts for all posts
UPDATE suika.Posts 
SET Excerpt = (
    SELECT str_value 
    FROM source.blog_fields 
    WHERE source.blog_fields.cid = suika.Posts.PostID
    AND name = 'excerpt'
);

-- Updating Banner for all posts
UPDATE suika.Posts 
SET Banner = (
    SELECT str_value 
    FROM source.blog_fields 
    WHERE source.blog_fields.cid = suika.Posts.PostID 
    AND name = 'banner'
);

-- Replace the URL in banner to be relative link
UPDATE suika.Posts
SET Banner = SUBSTR(Banner, 1, INSTR(Banner, '#') - 1)
WHERE INSTR(Banner, '#') > 0;

UPDATE suika.Posts
SET Banner = SUBSTR(Banner, 1, INSTR(Banner, '#') - 1)
WHERE INSTR(Banner, '#') > 0;

UPDATE suika.Posts
SET Banner = SUBSTR(BANNER, INSTR(BANNER, 'uploads/') + 8)
WHERE BANNER LIKE '%/uploads/%';

UPDATE suika.Posts
SET Banner = SUBSTR(BANNER, INSTR(BANNER, 'VOID/assets/') + 12)
WHERE BANNER LIKE '%VOID/assets/%';

-- Replace the URL in content to be relative link
UPDATE suika.Posts
SET Content = SUBSTR(BANNER, INSTR(BANNER, 'VOID/assets/') + 12)
WHERE BANNER LIKE '%VOID/assets/%';


-- Update Comment table
UPDATE source.blog_comments
SET mail = 'simonli666@qq.com'
WHERE author = '野兽后辈';

UPDATE source.blog_comments
SET mail = 'twisland@outlook.com'
WHERE author = 'TwinIsland';

INSERT INTO suika.Visitors (name, email, website, ip)
SELECT DISTINCT author, mail, url, ip
FROM source.blog_comments
WHERE author IS NOT NULL AND mail IS NOT NULL
GROUP BY author, mail;


INSERT INTO suika.Comment (postID, AuthorName, createDate, content, upVoted)
SELECT cid, source.blog_comments.author, datetime(created, 'unixepoch'), text, likes
FROM source.blog_comments
WHERE cid != 598;

-- Update Metas 
INSERT INTO suika.Meta (Name, type)
SELECT name, type
FROM source.blog_metas;

INSERT INTO suika.PostMeta (PostID, MetaName)
SELECT br.cid, bm.name
FROM source.blog_relationships AS br
JOIN source.blog_metas AS bm ON bm.mid = br.mid;


-- SELECT Banner FROM suika.Posts