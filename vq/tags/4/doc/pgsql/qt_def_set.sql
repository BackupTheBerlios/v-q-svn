/*
 * Set default quota value for users in given domain. Or for all new
 * domains (if name is '-')
 */

CREATE OR REPLACE FUNCTION qt_def_set(TEXT,INT,INT) RETURNS VOID AS '
DECLARE
	dom ALIAS FOR $1;
	qb ALIAS FOR $2;
	qf ALIAS FOR $3;
	ex RECORD;
BEGIN
	FOR ex IN EXECUTE ''SELECT 1 FROM domains WHERE name=''
		|| quote_literal(dom) || '' LIMIT 1'' LOOP
	
		EXECUTE ''UPDATE domains SET qt_user_bytes_def = '' || qb
			|| '', qt_user_files_def = '' || qf
			|| '' WHERE name = '' || quote_literal(dom);
	
		RETURN;
	END LOOP;

	EXECUTE ''INSERT INTO domains (name,qt_user_bytes_def,qt_user_files_def)''
		|| '' VALUES('' || quote_literal(dom) 
		|| '','' || qb || '','' || qf || '')'';

	RETURN;
END;
' LANGUAGE 'plpgsql';
