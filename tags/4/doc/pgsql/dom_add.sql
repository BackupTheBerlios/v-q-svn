CREATE or replace FUNCTION "dom_add" (text,text) RETURNS boolean AS '
declare
    dom alias for $1;
    user alias for $2;
begin
    EXECUTE ''create table '' || quote_ident(dom)
            || '' (id serial, login text unique not null check ''
            || ''(char_length(login)!=0), pass text not null, ''
            || ''flags int default 0 not null,''
            || ''ftpdir text,ftpuid int,''
            || ''ftpgid int,''
            || ''ftpsubdir text,''
            || ''date_crt date not null default now(),''
            || '' primary key(id))''; 

	PERFORM dom_log_add(dom,user);
	
    EXECUTE ''create table '' || quote_ident(dom || ''_dot'' )
            || '' (id serial, type char(1),''
            || ''value text not null,''
            || ''ext text not null,''
            || ''uid int CONSTRAINT '' || quote_ident(dom || ''_dot_uid_fkey'') 
            || '' REFERENCES '' || quote_ident(dom) 
            || '' (id) ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE,''
            || ''primary key(id))'';
    EXECUTE ''create index '' || quote_ident(dom || ''_dot_type_idx'')
            || '' ON '' || quote_ident(dom || ''_dot'') || '' (type)'';
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom)
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_dot'')
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_id_seq'')
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_dot_id_seq'')
            || '' TO '' || user;
    RETURN ''t''::boolean;
end;
' LANGUAGE 'plpgsql';
