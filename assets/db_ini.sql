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
    Banned INTEGER
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