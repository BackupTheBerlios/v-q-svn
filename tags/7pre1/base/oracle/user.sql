CREATE USER "MAIL" PROFILE "DEFAULT" IDENTIFIED BY "*******" DEFAULT TABLESPACE "MAIL" TEMPORARY TABLESPACE "TEMP" ACCOUNT UNLOCK;
GRANT UNLIMITED TABLESPACE TO "MAIL";
GRANT "CONNECT" TO "MAIL";
GRANT "RESOURCE" TO "MAIL";
