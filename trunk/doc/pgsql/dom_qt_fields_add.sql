/* 
 * This script adds new field to domain table which store informations about
 * quota for each user.
 */

/*
 * Get field from domains table for specified domain/table.
 */
CREATE OR REPLACE FUNCTION dom_qt_def_get(TEXT,TEXT) RETURNS INT AS '
DECLARE
	domain ALIAS FOR $1;
	field ALIAS FOR $2;
	def RECORD;
BEGIN
	FOR def IN EXECUTE ''SELECT '' || quote_ident(field) 
			|| '' AS def FROM domains''
			|| '' WHERE name ILIKE '' || quote_literal(domain) LOOP
		RETURN def.def;
	END LOOP;

	RETURN 0;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION dom_qt_fields_add(TEXT) RETURNS VOID AS '
DECLARE
	table ALIAS FOR $1;
BEGIN
	EXECUTE ''ALTER TABLE '' || quote_ident(table) 
		|| '' ADD qt_files INT'';

	EXECUTE ''ALTER TABLE '' || quote_ident(table)
		|| '' ADD qt_bytes INT'';

	EXECUTE ''UPDATE '' || quote_ident(table) || '' SET ''
		|| ''qt_files=''
		|| ''(SELECT qt_user_files_def FROM domains''
			|| '' WHERE name ILIKE '' || quote_literal(table) || '' LIMIT 1),''
		|| ''qt_bytes=''
		|| ''(SELECT qt_user_bytes_def FROM domains''
			|| '' WHERE name ILIKE '' || quote_literal(table) || '' LIMIT 1)'';

	EXECUTE ''ALTER TABLE '' || quote_ident(table)
		|| '' ALTER qt_files SET NOT NULL'';

	EXECUTE ''ALTER TABLE '' || quote_ident(table)
		|| '' ALTER qt_bytes SET NOT NULL'';

	EXECUTE ''ALTER TABLE '' || quote_ident(table)
		|| '' ALTER qt_files SET DEFAULT ''
		|| ''DOM_QT_DEF_GET('' || quote_literal(table) || '',''
			|| quote_literal(''qt_user_files_def'') || '')'';

	EXECUTE ''ALTER TABLE '' || quote_ident(table)
		|| '' ALTER qt_bytes SET DEFAULT ''
		|| ''DOM_QT_DEF_GET('' || quote_literal(table) || '',''
			|| quote_literal(''qt_user_bytes_def'') || '')'';

	RETURN;
END;
' LANGUAGE 'plpgsql';
