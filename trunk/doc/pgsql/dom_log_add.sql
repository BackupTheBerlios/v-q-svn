/*
Function creates table for logging purposes which is assiociated with
given table (domain)
*/

CREATE or replace FUNCTION "dom_log_add" (text,text) RETURNS boolean AS '
declare
    dom alias for $1;
	user alias for $2;
begin
    EXECUTE ''create table '' || quote_ident(dom || ''_log'' )
            || '' (ID serial,''
			|| ''TIME timestamp not null default now(),''
            || ''LOGIN text not null default '' || quote_literal('''') || '','' 
            || ''SERVICE smallint not null default 0,''
            || ''RESULT smallint not null default 0,'' 
            || ''IP text not null default '' || quote_literal('''') || '',''
			|| ''MSG text not null default '' || quote_literal('''') || '',''
            || ''PRIMARY KEY(id))'';

    EXECUTE ''CREATE INDEX '' || quote_ident(dom || ''_log_login_idx'')
            || '' ON '' || quote_ident(dom || ''_log'') || '' (login)'';
	
	EXECUTE ''CREATE INDEX '' || quote_ident(dom || ''_log_service_idx'')
            || '' ON '' || quote_ident(dom || ''_log'') || '' (service)'';
		
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_log'')
            || '' TO '' || user;
			
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_log_id_seq'')
            || '' TO '' || user;
			
    RETURN ''t''::boolean;
end;
' LANGUAGE 'plpgsql';
