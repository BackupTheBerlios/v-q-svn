CREATE or replace FUNCTION mail.dom_rm (text) RETURNS VOID AS '
declare
    dom alias for $1;
begin
	EXECUTE ''DELETE FROM domains WHERE domain ='' || quote_literal(dom);
	NOTIFY dom_rm;
	RETURN;
end;
' LANGUAGE 'plpgsql';
