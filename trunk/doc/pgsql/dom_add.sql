CREATE OR REPLACE FUNCTION mail.dom_add (text) RETURNS INT AS '
declare
    dom ALIAS FOR $1;
	r RECORD;
begin
	FOR r IN EXECUTE ''SELECT 1::integer FROM domains WHERE domain='' 
		|| quote_literal(dom) LOOP
		RETURN 2;
	END LOOP;
	
	EXECUTE ''INSERT INTO domains (domain) VALUES('' 
			|| quote_literal(dom) || '')'';
	NOTIFY dom_add;
	RETURN 1;
END;
' LANGUAGE 'plpgsql';
