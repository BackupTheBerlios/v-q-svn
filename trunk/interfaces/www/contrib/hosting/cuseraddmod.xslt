<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/useradd">Dodaj u�ytkownika</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/useradd">
	Tutaj mo�esz doda�
	konto pocztowe, kt�re b�dzie mia�o ograniczon� wa�no�� po uiszczeniu
	stosownej op�aty wa�no�� konta zostanie przed�u�ona.
</xsl:template>

<xsl:template name="useradd_err">
	<xsl:if test="inv/login/@empty">
		<tr class="err"><td>Podaj nazw� u�ytkownika.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/login/@dirty">
		<tr class="err"><td>Nazwa u�ytkownika zawiera nieprawid�owe znaki.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/login/@tooshort">
		<tr class="err"><td>Nazwa u�ytkownika jest za kr�tka.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/login/@toolong">
		<tr class="err"><td>Nazwa u�ytkownika jest za d�uga.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/pass/@empty">
		<tr class="err"><td>Podaj has�o.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/pass/@dirty">
		<tr class="err"><td>Has�o zawiera nieprawid�owe znaki.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/pass/@tooshort">
		<tr class="err"><td>Has�o jest za kr�tkie.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/pass/@toolong">
		<tr class="err"><td>Has�o jest za d�ugie.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/dontmatch">
		<tr class="err"><td>Has�o i powt�rka s� r�ne.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/cant">
		<tr class="err"><td>Nie mo�na doda� u�ytkownika, je�eli sytuacja powtarza si� powiadom administratora.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/done">
		<tr class="err"><td>Dodano u�ytkownika.</td></tr>
	</xsl:if>
	
	<xsl:if test="inv/inv">
		<tr class="err"><td>Nieprawid�owa nazwa u�ytkownika.</td></tr>
	</xsl:if>
</xsl:template>

<xsl:template match="vqwww/mod/useradd">
	<form method="post">
		<table align="center">
			<xsl:if test="inv">
				<xsl:call-template name="useradd_err"/>
			</xsl:if>

			<tr>
				<td>Nazwa u�ytkownika:</td>
				<td>
					<input type="text" name="login">
						<xsl:attribute name="value">
							<xsl:value-of select="login/@val"/>
						</xsl:attribute>
					</input>
				</td>
			</tr>

			<tr>
				<td>Has�o:</td>
				<td><input type="password" name="pass"/></td>
			</tr>

			<tr>
				<td>Powt�rka has�a:</td>
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
