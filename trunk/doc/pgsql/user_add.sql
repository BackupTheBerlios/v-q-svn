CREATE or replace FUNCTION "user_add" (text,text,text,int4) RETURNS int AS '
declare
    user alias for $1;
    dom alias for $2;
    pass alias for $3;
    flags alias for $4;
    users RECORD;
    first int := 4;
    id RECORD;
    ban RECORD;
begin
    IF flags & 1 = 0 THEN
      FOR ban IN SELECT 1 FROM login_banned WHERE login=user LOOP
        RETURN 1;
      END LOOP;
    END IF;
    FOR users IN EXECUTE ''SELECT id FROM '' || quote_ident(dom) 
            || '' WHERE login!=''|| quote_literal(''ftp'') ||'' LIMIT 1'' LOOP
            first := 0;
    END LOOP;
    FOR id IN EXECUTE ''SELECT nextval('' || quote_literal(dom || ''_id_seq'') || '')'' LOOP 
            EXECUTE ''insert into '' || quote_ident(dom) 
                    || '' (id,login,pass,flags) values('' 
                    || quote_literal(id.nextval)
                    || '','' || quote_literal(user) 
                    || '','' || quote_literal(pass) 
                    || '','' || first || '')'';
            EXECUTE ''insert into '' || quote_ident(dom || ''_dot'')
                    || '' (type,value,ext,uid) ''
                    || ''VALUES(''|| quote_literal(''M'') 
                    || '','' || quote_literal(''./'' || user || ''/Maildir/'') 
                    || '','' || quote_literal(user || ''-'')  
                    ||'','' || quote_literal(id.nextval) || '')'';
            RETURN 0;
    END LOOP;
    RAISE EXCEPTION ''select nextval'';
end;
' LANGUAGE 'plpgsql';
