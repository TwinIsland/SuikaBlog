PRAGMA foreign_keys = ON;

CREATE TABLE Posts (
    PostID INTEGER PRIMARY KEY AUTOINCREMENT,
    Title VARCHAR(255) NOT NULL,
    Banner TEXT,
    Excerpt TEXT,
    Content TEXT NOT NULL,
    CreateDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    DateModified DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UpVoted INT NOT NULL DEFAULT 0,
    Views INT DEFAULT 0
);

CREATE TABLE Visitors (
    VisitorID INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT NOT NULL UNIQUE,
    Email TEXT,
    Website TEXT,
    Banned INTEGER,
    Ip VARCHAR(64) NOT NULL,
    UNIQUE(Name, Email)
);

CREATE TABLE Comment (
    CommentID INTEGER PRIMARY KEY AUTOINCREMENT,
    PostID INTEGER NOT NULL,
    VisitorID INTEGER, 
    CreateDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    Content TEXT NOT NULL,
    UpVoted INTEGER DEFAULT 0,
    FOREIGN KEY (PostID) REFERENCES Posts(PostID) ON DELETE CASCADE,
    FOREIGN KEY (VisitorID) REFERENCES Visitors(VisitorID) ON DELETE CASCADE
);

CREATE TABLE CommentPage (
    CommentID INTEGER PRIMARY KEY AUTOINCREMENT,
    VisitorID INTEGER, 
    PageName VARCHAR(64) NOT NULL,
    CreateDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    Content TEXT NOT NULL,
    UpVoted INTEGER DEFAULT 0,
    FOREIGN KEY (VisitorID) REFERENCES Visitors(VisitorID) ON DELETE CASCADE
);

CREATE TABLE Activity (
    ActivityID INTEGER PRIMARY KEY AUTOINCREMENT,
    VisitorID INTEGER,
    Description TEXT NOT NULL,
    CreateDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (VisitorID) REFERENCES Visitors(VisitorID)
);

CREATE TABLE Meta (
    MetaID INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT UNIQUE NOT NULL,
    Type VARCHAR(32) NOT NULL
);

CREATE TABLE PostMeta (
    PostID INTEGER,
    MetaID INTEGER,
    PRIMARY KEY (PostID, MetaID),
    FOREIGN KEY (PostID) REFERENCES Posts(PostID) ON DELETE CASCADE,
    FOREIGN KEY (MetaID) REFERENCES Meta(MetaID) ON DELETE CASCADE
);


-- Triggers
CREATE TRIGGER InsertCommentActivity AFTER INSERT ON Comment
BEGIN
    INSERT INTO Activity (visitorID, description, createDate)
    SELECT NEW.visitorID, 'comment on post: ' || Posts.Title, NEW.createDate
    FROM Posts
    WHERE Posts.PostID = NEW.postID;
END;

CREATE TRIGGER InsertCommentPageActivity AFTER INSERT ON CommentPage
BEGIN
    INSERT INTO Activity (visitorID, description, createDate)
    VALUES (NEW.visitorID, 'comment on page: ' || NEW.pageName, NEW.createDate);
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


INSERT INTO suika.Comment (postID, visitorID, createDate, content, upVoted)
SELECT cid, (SELECT visitorID FROM suika.Visitors WHERE name = source.blog_comments.author), datetime(created, 'unixepoch'), text, likes
FROM source.blog_comments
WHERE cid IN (SELECT PostID FROM suika.Posts);



INSERT INTO suika.CommentPage (visitorID, pageName, createDate, content, upVoted)
SELECT (SELECT visitorID FROM suika.Visitors WHERE name = source.blog_comments.author), CAST(cid AS VARCHAR(64)), datetime(created, 'unixepoch'), text, likes
FROM source.blog_comments
WHERE cid NOT IN (SELECT PostID FROM suika.Posts)
AND cid != 598;


-- Update Comment PageName
UPDATE suika.CommentPage
SET pageName = "About"
WHERE pageName = "9";

UPDATE suika.CommentPage
SET pageName = "Lucky"
WHERE pageName = "344";

UPDATE suika.CommentPage
SET pageName = "Friends"
WHERE pageName = "433";

-- Update Metas 
INSERT INTO suika.Meta (MetaID, Name, type)
SELECT DISTINCT mid, name, type
FROM source.blog_metas;

INSERT INTO suika.PostMeta (PostID, MetaID)
SELECT cid, mid
FROM source.blog_relationships;


