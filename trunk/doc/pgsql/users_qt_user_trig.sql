CREATE OR REPLACE FUNCTION mail.USERS_QT_USER_TRIG () RETURNS TRIGGER AS '
BEGIN
	IF NEW.qt_files_max = 0 THEN
		NEW.qt_files_max := (SELECT qt_user_files_def FROM domains WHERE id_domain=NEW.id_domain);
	END IF;

	IF NEW.qt_bytes_max = 0 THEN
		NEW.qt_bytes_max := (SELECT qt_user_bytes_def FROM domains WHERE id_domain=NEW.id_domain);
	END IF;

	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

DROP TRIGGER users_qt_user_trig ON mail.users;
CREATE TRIGGER users_qt_user_trig BEFORE INSERT ON users 
FOR EACH ROW EXECUTE PROCEDURE mail.USERS_QT_USER_TRIG ();
