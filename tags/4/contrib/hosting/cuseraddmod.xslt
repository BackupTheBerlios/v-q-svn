<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/useradd">Dodaj użytkownika</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/useradd">
	Tutaj możesz dodać
	konto pocztowe, które będzie miało ograniczoną ważność po uiszczeniu
	stosownej opłaty ważność konta zostanie przedłużona.
</xsl:template>

<xsl:template name="useradd_err">
	<xsl:if test="inv/login/@empty">
		<tr class="err"><td>Podaj nazwę użytkownika.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/login/@dirty">
		<tr class="err"><td>Nazwa użytkownika zawiera nieprawidłowe znaki.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/login/@tooshort">
		<tr class="err"><td>Nazwa użytkownika jest za krótka.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/login/@toolong">
		<tr class="err"><td>Nazwa użytkownika jest za długa.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/pass/@empty">
		<tr class="err"><td>Podaj hasło.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/pass/@dirty">
		<tr class="err"><td>Hasło zawiera nieprawidłowe znaki.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/pass/@tooshort">
		<tr class="err"><td>Hasło jest za krótkie.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/pass/@toolong">
		<tr class="err"><td>Hasło jest za długie.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/dontmatch">
		<tr class="err"><td>Hasło i powtórka są różne.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/cant">
		<tr class="err"><td>Nie można dodać użytkownika, jeżeli sytuacja powtarza się powiadom administratora.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/done">
		<tr class="err"><td>Dodano użytkownika.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/inv">
		<tr class="err"><td>Nieprawidłowa nazwa użytkownika.</td></tr>
	</xsl:if>
</xsl:template>

<xsl:template match="vqwww/mod/useradd">
	<form method="post">
		<table align="center">
			<xsl:if test="inv">
				<xsl:call-template name="useradd_err"/>
			</xsl:if>

			<tr>
				<td>Nazwa użytkownika:</td>
				<td>
					<input type="text" name="login">
						<xsl:attribute name="value">
							<xsl:value-of select="login/@val"/>
						</xsl:attribute>
					</input>
				</td>
			</tr>

			<tr>
				<td>Hasło:</td>
				<td><input type="password" name="pass"/></td>
			</tr>

			<tr>
				<td>Powtórka hasła:</td>
				<td><input type="password" name="pass1"/></td>
			</tr>
			
			<tr>
				<td align="center" colspan="2">
					<input class="button" type="submit" value="Dodaj"/>
				</td>
			</tr>

			<input type="hidden" name="id" value="useradd"/>
			<input type="hidden" name="conf" value="1"/>
		</table>
	</form>
</xsl:template>

</xsl:stylesheet>
