CREATE OR REPLACE FUNCTION mail.QT_USER_DEF_GET(mail.domains.id_domain%TYPE) RETURNS RECORD AS '
DECLARE
	_id_domain ALIAS FOR $1;
	qt RECORD;
BEGIN
	SELECT INTO qt qt_user_bytes_def,qt_user_files_def FROM domains 
		WHERE id_domain=_id_domain LIMIT 1;
	IF NOT FOUND THEN
		SELECT INTO qt null::int,null::int;
	END IF;
	RETURN qt;
END;
' LANGUAGE 'plpgsql';
