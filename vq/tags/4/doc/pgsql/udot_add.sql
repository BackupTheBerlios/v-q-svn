CREATE or replace FUNCTION "udot_add" (text,text,text,char,text) RETURNS int4 AS '
declare
    dom alias for $1;
    user alias for $2;
    ext alias for $3;
    tp alias for $4;
    val alias for $5;
    nv RECORD;
begin
    FOR nv IN EXECUTE ''SELECT nextval('' || quote_literal(dom 
                || ''_dot_id_seq'') || '')'' LOOP
            
            EXECUTE ''INSERT INTO '' || quote_ident(dom || ''_dot'') 
                || '' (ID,TYPE,VALUE,EXT,UID) VALUES(''
                || quote_literal(nv.nextval) || '',''
                || quote_literal(tp) || '',''
                || quote_literal(val) || '',''
                || quote_literal(user || ''-'' || ext) 
                || '',(SELECT id FROM ''|| quote_ident(dom) 
                ||'' WHERE login='' || quote_literal(user) || '' LIMIT 1))'';

            RETURN nv.nextval;
    END LOOP;
    RAISE EXCEPTION ''????'';
end;
' LANGUAGE 'plpgsql';
