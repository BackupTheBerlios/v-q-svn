/*
 * Set default quota value for users in given domain.
 */

CREATE OR REPLACE FUNCTION mail.qt_user_set
(mail.users.id_domain%TYPE,mail.users.id_user%TYPE,
mail.users.qt_bytes_max%TYPE,mail.users.qt_files_max%TYPE) RETURNS VOID AS '
DECLARE
	_id_domain ALIAS FOR $1;
	_id_user ALIAS FOR $2;
	bytes_max ALIAS FOR $3;
	files_max ALIAS FOR $4;
BEGIN
	UPDATE users SET qt_bytes_max=bytes_max,qt_files_max=files_max
		WHERE id_domain=_id_domain AND id_user=_id_user;
	NOTIFY qt_user_set;
	RETURN;
END;
' LANGUAGE 'plpgsql';
