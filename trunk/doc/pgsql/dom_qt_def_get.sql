/*
 * Get field from domains table for specified domain/table.
 */
CREATE OR REPLACE FUNCTION mail.dom_qt_def_get(TEXT,TEXT) RETURNS INT AS '
DECLARE
	domain ALIAS FOR $1;
	field ALIAS FOR $2;
	def RECORD;
BEGIN
	FOR def IN EXECUTE ''SELECT '' || quote_ident(field) 
			|| '' AS def FROM domains''
			|| '' WHERE domain = '' || quote_literal(domain) LOOP
		RETURN def.def;
	END LOOP;

	RETURN 0;
END;
' LANGUAGE 'plpgsql';

