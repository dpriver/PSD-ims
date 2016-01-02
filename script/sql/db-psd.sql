CREATE DATABASE PSD;

USE PSD;

/*--------------------------CREATE TABLE---------------------------------------*/

CREATE TABLE users(
 ID INT NOT NULL AUTO_INCREMENT,
 VALID INT,
 NAME VARCHAR(25) NOT NULL UNIQUE,
 PASS VARCHAR(25) NOT NULL,
 INFORMATION VARCHAR(100),
 PRIMARY KEY (ID)
);

CREATE TABLE chats(
 ID INT(10) NOT NULL AUTO_INCREMENT,
 ID_ADMIN INT(10) NOT NULL,
 ADMIN_TIME INT(10),
 READ_TIME INT(10),
 VALID INT,
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
 READ_MSG_TIME INT(10),
 REM_TIME INT(10),
 FOREIGN KEY (ID_USERS) REFERENCES users(ID) on delete cascade on update cascade,
 FOREIGN KEY (ID_CHAT) REFERENCES chats(ID) on delete cascade on update cascade
);

CREATE TABLE messages(
 ID_SENDER INT(10) NOT NULL, 
 ID_CHAT INT(10) NOT NULL,
 FILE_ VARCHAR(50),
 TEXT VARCHAR(500),
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

insert into users (NAME, PASS, INFORMATION) values ('System',  '', 'Sup, im da real system');
