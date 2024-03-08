PRAGMA foreign_keys = ON;

CREATE TABLE Posts (
    PostID INTEGER PRIMARY KEY AUTOINCREMENT,
    Title VARCHAR(255) NOT NULL,
    Banner TEXT,
    Excerpt TEXT,
    Content TEXT NOT NULL,
    DatePublished DATETIME DEFAULT CURRENT_TIMESTAMP,
    DateModified DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UpVoted INT NOT NULL DEFAULT 0,
    Views INT DEFAULT 0
);

CREATE TABLE Visitors (
    visitorID INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    email TEXT,
    website TEXT,
    ip TEXT NOT NULL
);

CREATE TABLE Comment (
    commentID INTEGER PRIMARY KEY AUTOINCREMENT,
    postID INTEGER NOT NULL,
    visitorID INTEGER, 
    content TEXT NOT NULL,
    upVoted INTEGER DEFAULT 0,
    downVoted INTEGER DEFAULT 0,
    FOREIGN KEY (postID) REFERENCES Posts(PostID),
    FOREIGN KEY (visitorID) REFERENCES Visitors(visitorID)
);

CREATE TABLE Activity (
    activityID INTEGER PRIMARY KEY AUTOINCREMENT,
    visitorID INTEGER,
    type TEXT NOT NULL,
    createDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (visitorID) REFERENCES Visitors(visitorID)
);

CREATE TABLE Tag (
    tagID INTEGER PRIMARY KEY AUTOINCREMENT,
    tagName TEXT UNIQUE NOT NULL
);

CREATE TABLE Category (
    categoryID INTEGER PRIMARY KEY AUTOINCREMENT,
    categoryName TEXT UNIQUE NOT NULL
);

CREATE TABLE PostTags (
    PostID INTEGER,
    TagID INTEGER,
    PRIMARY KEY (PostID, TagID),
    FOREIGN KEY (PostID) REFERENCES Posts(PostID) ON DELETE CASCADE,
    FOREIGN KEY (TagID) REFERENCES Tag(TagID) ON DELETE CASCADE
);

CREATE TABLE PostCategories (
    PostID INTEGER,
    CategoryID INTEGER,
    PRIMARY KEY (PostID, CategoryID),
    FOREIGN KEY (PostID) REFERENCES Posts(PostID) ON DELETE CASCADE,
    FOREIGN KEY (CategoryID) REFERENCES Category(CategoryID) ON DELETE CASCADE
);


-- attaching the databases
ATTACH DATABASE 'source.db' AS source;
ATTACH DATABASE 'suika.db' AS suika;

-- Inserting posts, filtering out pages and titles with periods
INSERT INTO suika.Posts (PostID, Title, Content, DatePublished, DateModified, UpVoted, Views)
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

-- Replace the URL to be relative one
UPDATE suika.Posts
SET Banner = SUBSTR(Banner, 1, INSTR(Banner, '#') - 1)
WHERE INSTR(Banner, '#') > 0;
