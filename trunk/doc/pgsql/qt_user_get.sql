CREATE OR REPLACE FUNCTION mail.qt_user_get
(mail.users.id_domain%TYPE,mail.users.id_user%TYPE) RETURNS RECORD AS '
DECLARE
	_id_domain ALIAS FOR $1;
	_id_user ALIAS FOR $2;
	qt RECORD;
BEGIN
	SELECT INTO qt qt_bytes_max,qt_files_max FROM users
		WHERE id_domain=_id_domain AND id_user=_id_user LIMIT 1;
	IF NOT FOUND THEN
		SELECT INTO qt null::int,null::int; 
	END IF;
	RETURN qt;
END;
' LANGUAGE 'plpgsql';
