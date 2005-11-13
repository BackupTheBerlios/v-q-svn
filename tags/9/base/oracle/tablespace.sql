CREATE SMALLFILE TABLESPACE "MAIL" DATAFILE '/db/oracle/oradata/orcl/mail01.dbf' SIZE 30M REUSE AUTOEXTEND ON NEXT 30720K MAXSIZE 32767M LOGGING EXTENT MANAGEMENT LOCAL SEGMENT SPACE MANAGEMENT AUTO;
BEGIN 
DBMS_SERVER_ALERT.SET_THRESHOLD(9000,NULL,NULL,NULL,NULL,1,1,NULL,5,'MAIL'); 
END;
/
