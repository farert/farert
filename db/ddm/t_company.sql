BEGIN TRANSACTION;
CREATE TABLE t_company (
	name char(11) not null primary key
);

INSERT INTO [t_company] ([name]) VALUES ('JR�k�C��');
INSERT INTO [t_company] ([name]) VALUES ('JR�����{');
INSERT INTO [t_company] ([name]) VALUES ('JR���C');
INSERT INTO [t_company] ([name]) VALUES ('JR�����{');
INSERT INTO [t_company] ([name]) VALUES ('JR��B');
INSERT INTO [t_company] ([name]) VALUES ('JR�l��');
COMMIT;
