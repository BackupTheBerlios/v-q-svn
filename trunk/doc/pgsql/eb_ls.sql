CREATE or replace FUNCTION mail.eb_ls () RETURNS SETOF emails_banned AS '
declare
	ban RECORD;
begin
	FOR ban IN SELECT * FROM emails_banned LOOP
		RETURN NEXT ban;
	END LOOP;
	RETURN;
end;
' LANGUAGE 'plpgsql';
