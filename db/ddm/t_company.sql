BEGIN TRANSACTION;
CREATE TABLE t_company (
	name char(11) not null primary key
);

INSERT INTO [t_company] ([name]) VALUES ('JRñkäCìπ');
INSERT INTO [t_company] ([name]) VALUES ('JRìåì˙ñ{');
INSERT INTO [t_company] ([name]) VALUES ('JRìåäC');
INSERT INTO [t_company] ([name]) VALUES ('JRêºì˙ñ{');
INSERT INTO [t_company] ([name]) VALUES ('JRã„èB');
INSERT INTO [t_company] ([name]) VALUES ('JRélçë');
COMMIT;
