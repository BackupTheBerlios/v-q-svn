CREATE OR REPLACE FUNCTION mail.DOMAINS_QT_USER_TRIG () RETURNS TRIGGER AS '
DECLARE
	qt RECORD;
BEGIN
	SELECT INTO qt qt_user_files_def,qt_user_bytes_def FROM domains 
		WHERE domain=''global'';

	IF FOUND THEN
		IF NEW.qt_user_files_def = 0 THEN
			NEW.qt_user_files_def := qt.qt_user_files_def;
		END IF;

		IF NEW.qt_user_bytes_def = 0 THEN
			NEW.qt_user_bytes_def := qt.qt_user_bytes_def;
		END IF;
	END IF;

	RETURN NEW;
END;
' LANGUAGE 'plpgsql';

DROP TRIGGER domains_qt_user_trig ON mail.domains;
CREATE TRIGGER domains_qt_user_trig BEFORE INSERT ON domains 
FOR EACH ROW EXECUTE PROCEDURE mail.DOMAINS_QT_USER_TRIG ();
