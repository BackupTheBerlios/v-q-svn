<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/ftp">U�ytkownicy FTP</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/ftp">
	Tutaj mo�esz przyzna� dost�p u�ytkownikom do wyszczeg�lnionych cz�ci
	serwisu WWW.
</xsl:template>

<xsl:template name="ftp_table">
	<table align="center">
		<tr>
			<th>U�ytkownik</th>
			<th>Katalog FTP</th>
		</tr>


		<xsl:for-each select="item">
			<tr>
				<td>
					<xsl:value-of select="@user"/>
				</td>
				<td>
					<input>
						<xsl:attribute name="name">
							<xsl:text>item</xsl:text>
							<xsl:value-of select="position()"/>
						</xsl:attribute>
						<xsl:attribute name="value">
							<xsl:value-of select="@val"/>
						</xsl:attribute>
					</input>
					<input type="hidden">
						<xsl:attribute name="name">
							<xsl:text>itemid</xsl:text>
							<xsl:value-of select="position()"/>
						</xsl:attribute>
						<xsl:attribute name="value">
							<xsl:value-of select="@id"/>
						</xsl:attribute>
					</input>
					<input type="hidden">
						<xsl:attribute name="name">
							<xsl:text>itemuser</xsl:text>
							<xsl:value-of select="position()"/>
						</xsl:attribute>
						<xsl:attribute name="value">
							<xsl:value-of select="@user"/>
						</xsl:attribute>
					</input>
				</td>
			</tr>
		</xsl:for-each>

		<tr>
			<td align="center" colspan="2">
				<input class="button" type="submit" value="Zmie�"/>
			</td>
		</tr>

	</table>
</xsl:template>

<xsl:template match="vqwww/mod/ftp">
	<form method="post">
		<table border="0">
			<xsl:choose>
				<xsl:when test="getcant">
					<tr class="err"><td>Nie mo�na pobra� aktualnej listy.</td></tr>
				</xsl:when>
				<xsl:otherwise>
					<tr class="desc">
						<td>
							Uwaga: aby umo�liwi� u�ytkownikowi dost�p do strony nie wystarczy tutaj
							poda� odpowiedniego katalogu-nale�y go r�wnie� utworzy�. Program
							nie sprawdza czy katalog istnieje; nie mo�liwe jest ustawienie dost�pu do
							kilku katalog�w; u�ytkownik ma dost�p tylko do wyznaczonego katalogu
							i podkatalog�w (nie ma dost�pu do katalog�w nadrz�dnych).
						</td>
					</tr>

					<!-- error messages -->
					<xsl:for-each select="inv/rm">
						<tr class="err">
							<td align="center">
								<xsl:text>Nie mo�na usun��: u�ytkownik </xsl:text>
								<xsl:value-of select="@user"/>
								<xsl:text>, katalog: </xsl:text>
								<xsl:value-of select="@dir"/>
							</td>
						</tr>
					</xsl:for-each>

					<xsl:for-each select="inv/dir">
						<tr class="err">
							<td align="center">
								<xsl:text>Nieprawid�owy katalog: u�ytkownik </xsl:text>
								<xsl:value-of select="@user"/>
								<xsl:text>, katalog: </xsl:text>
								<xsl:value-of select="@dir"/>
							</td>
						</tr>
					</xsl:for-each>

					<xsl:for-each select="inv/dirchars">
						<tr class="err">
							<td align="center">
								<xsl:text>Katalog zawiera� nieprawid�owe znaki: u�ytkownik </xsl:text>
								<xsl:value-of select="@user"/>
								<xsl:text>, katalog: </xsl:text>
								<xsl:value-of select="@dir"/>
							</td>
						</tr>
					</xsl:for-each>

					<xsl:for-each select="inv/rep">
						<tr class="err">
							<td align="center">
								<xsl:text>Nie mo�na zamieni�: u�ytkownik </xsl:text>
								<xsl:value-of select="@user"/>
								<xsl:text>, katalog: </xsl:text>
								<xsl:value-of select="@dir"/>
							</td>
						</tr>
					</xsl:for-each>

					<!-- input form -->
					<tr>
						<td>
							<xsl:call-template name="ftp_table"/>
						</td>
					</tr>
					
					<input type="hidden" name="id" value="ftp"/>
					<input type="hidden" name="conf" value="1"/>
				</xsl:otherwise>
			</xsl:choose>
		</table>
	</form>
</xsl:template>

</xsl:stylesheet>
