CREATE OR REPLACE FUNCTION "dom_add" (text,text,text) RETURNS VOID AS '
declare
    tab ALIAS FOR $1;
	dom ALIAS FOR $2;
    user ALIAS FOR $3;
begin
	EXECUTE ''INSERT INTO domains (name) VALUES('' 
			|| quote_literal(dom) || '')'';
			
    EXECUTE ''create table '' || quote_ident(tab)
            || '' (id serial, login text unique not null check ''
            || ''(char_length(login)!=0), pass text not null, ''
            || ''flags int default 0 not null,''
            || ''ftpdir text,ftpuid int,''
            || ''ftpgid int,''
            || ''ftpsubdir text,''
            || ''date_crt date not null default now(),''
            || '' primary key(id))''; 

	PERFORM dom_log_add(tab,user);
	
    EXECUTE ''create table '' || quote_ident(tab || ''_dot'' )
            || '' (id serial, type char(1),''
            || ''value text not null,''
            || ''ext text not null,''
            || ''uid int CONSTRAINT '' || quote_ident(tab || ''_dot_uid_fkey'') 
            || '' REFERENCES '' || quote_ident(tab) 
            || '' (id) ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE,''
            || ''primary key(id))'';
    EXECUTE ''CREATE INDEX '' || quote_ident(tab || ''_dot_type_idx'')
            || '' ON '' || quote_ident(tab || ''_dot'') || '' (type)'';
    EXECUTE ''GRANT ALL ON '' || quote_ident(tab)
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(tab || ''_dot'')
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(tab || ''_id_seq'')
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(tab || ''_dot_id_seq'')
            || '' TO '' || user;
	RETURN;
END;
' LANGUAGE 'plpgsql';
