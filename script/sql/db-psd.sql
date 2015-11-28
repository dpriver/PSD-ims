CREATE DATABASE PSD;

USE PSD;

/*--------------------------CREATE TABLE---------------------------------------*/

CREATE TABLE users(
 ID INT NOT NULL AUTO_INCREMENT,
 NAME VARCHAR(25) NOT NULL UNIQUE ,
 PASS VARCHAR(25) NOT NULL ,
 INFORMATION VARCHAR(100),
 PRIMARY KEY (ID)
);

CREATE TABLE chats(
 ID INT(10) NOT NULL AUTO_INCREMENT,
 ID_ADMIN INT(10) NOT NULL,
 DESCRIPTION VARCHAR(100),
 CREATION_TIME INT(10),
 PRIMARY KEY(ID),
 FOREIGN KEY (ID_ADMIN) REFERENCES users(ID) on delete cascade on update cascade
);

CREATE TABLE friends(
 ID1 INT(10) NOT NULL, 
 ID2 INT(10) NOT NULL,
 CREATION_TIME INT(10),
 FOREIGN KEY (ID1) REFERENCES users(ID) on delete cascade on update cascade,
 FOREIGN KEY (ID2) REFERENCES users(ID) on delete cascade on update cascade 
);

CREATE TABLE users_chats(
 ID_USERS INT(10) NOT NULL,
 ID_CHAT INT(10) NOT NULL,
 CREATION_TIME INT(10),
 FOREIGN KEY (ID_USERS) REFERENCES users(ID) on delete cascade on update cascade,
 FOREIGN KEY (ID_CHAT) REFERENCES chats(ID) on delete cascade on update cascade
);

CREATE TABLE messages(
 ID_SENDER INT(10) NOT NULL, 
 ID_CHAT INT(10) NOT NULL,
 FILE_ VARCHAR(500) ,
 TEXT VARCHAR(500) ,
 CREATION_TIME INT(10),
 FOREIGN KEY (ID_SENDER) REFERENCES users(ID) on delete cascade on update cascade,
 FOREIGN KEY (ID_CHAT) REFERENCES chats(ID) on delete cascade on update cascade
);

CREATE TABLE friends_request(
 ID1 INT(10) NOT NULL, 
 ID2_request INT(10) NOT NULL,
 CREATION_TIME INT(10),
 FOREIGN KEY (ID1) REFERENCES users(ID) on delete cascade on update cascade,
 FOREIGN KEY (ID2_request) REFERENCES users(ID) on delete cascade on update cascade 
);


/*--------------------------INSERT DATA---------------------------------------*/

/*TABLE USERS*/

INSERT INTO users(NAME,PASS,INFORMATION) VALUES('Pepe','contrasena1','Guapo y mas que guapo');
INSERT INTO users(NAME,PASS,INFORMATION) VALUES('Antonio','contrasena2','Feo');
INSERT INTO users(NAME,PASS,INFORMATION) VALUES('Maria','contrasena3','Gorda');
INSERT INTO users(NAME,PASS,INFORMATION) VALUES('Carmen','contrasena4','Delgada');
INSERT INTO users(NAME,PASS,INFORMATION) VALUES('Carlos','contrasena5','Indiferente');

/*TABLE CHATS*/

INSERT INTO chats(ID_ADMIN,DESCRIPTION) VALUES(1,'chat 1');
INSERT INTO chats(ID_ADMIN,DESCRIPTION) VALUES(1,'chat 2');
INSERT INTO chats(ID_ADMIN,DESCRIPTION) VALUES(2,'chat 3');

/*TABLE USER_CHATS*/

INSERT INTO users_chats(ID_USERS,ID_CHAT) VALUES(1,1);
INSERT INTO users_chats(ID_USERS,ID_CHAT) VALUES(1,2);
INSERT INTO users_chats(ID_USERS,ID_CHAT) VALUES(2,3);
INSERT INTO users_chats(ID_USERS,ID_CHAT) VALUES(3,3);
INSERT INTO users_chats(ID_USERS,ID_CHAT) VALUES(4,3);

/*TABLE FRIENS*/

INSERT INTO friends(ID1,ID2) VALUES(1,2);
INSERT INTO friends(ID1,ID2) VALUES(3,2);
INSERT INTO friends(ID1,ID2) VALUES(3,4);
INSERT INTO friends(ID1,ID2) VALUES(2,4);

/*TABLE REQUEST*/

INSERT INTO friends_request(ID1,ID2_request) VALUES(1,3);
INSERT INTO friends_request(ID1,ID2_request) VALUES(5,3);
INSERT INTO friends_request(ID1,ID2_request) VALUES(5,2);

/*TABLE MESSAGES*/

INSERT INTO messages(ID_SENDER,ID_CHAT,FILE_,TEXT, CREATION_TIME) VALUES(3,3,'fichero.txt','hola que tal guapo?',20);
INSERT INTO messages(ID_SENDER,ID_CHAT,FILE_,TEXT,CREATION_TIME) VALUES(4,3,'null','pues muy bien y tsdfu?',22);
INSERT INTO messages(ID_SENDER,ID_CHAT,FILE_,TEXT,CREATION_TIME) VALUES(4,3,'null','tuu?*',20);
INSERT INTO messages(ID_SENDER,ID_CHAT,FILE_,TEXT,CREATION_TIME) VALUES(1,1,'null','uuuuooola',15);
INSERT INTO messages(ID_SENDER,ID_CHAT,FILE_,TEXT,CREATION_TIME) VALUES(1,1,'null','uuuuooola*2',25);

