/*
 * Set default quota value for users in given domain.
 */

CREATE OR REPLACE FUNCTION mail.qt_user_def_set
(mail.domains.id_domain%TYPE,mail.domains.qt_user_bytes_def%TYPE,mail.domains.qt_user_files_def%TYPE) RETURNS VOID AS '
DECLARE
	_id_domain ALIAS FOR $1;
	bytes_max ALIAS FOR $2;
	files_max ALIAS FOR $3;
BEGIN
	UPDATE domains SET qt_user_bytes_def=bytes_max,qt_user_files_def=files_max
		WHERE id_domain=_id_domain;
	NOTIFY qt_user_def_set;
	RETURN;
END;
' LANGUAGE 'plpgsql';
