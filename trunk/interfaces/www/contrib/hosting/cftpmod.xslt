<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/ftp">Użytkownicy FTP</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/ftp">
	Tutaj możesz przyznać dostęp użytkownikom do wyszczególnionych części
	serwisu WWW.
</xsl:template>

<xsl:template name="ftp_table">
	<table align="center">
		<tr>
			<th>Użytkownik</th>
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
				<input class="button" type="submit" value="Zmień"/>
			</td>
		</tr>

	</table>
</xsl:template>

<xsl:template match="vqwww/mod/ftp">
	<form method="post">
		<table border="0">
			<xsl:choose>
				<xsl:when test="getcant">
					<tr class="err"><td>Nie można pobrać aktualnej listy.</td></tr>
				</xsl:when>
				<xsl:otherwise>
					<tr class="desc">
						<td>
							Uwaga: aby umożliwić użytkownikowi dostęp do strony nie wystarczy tutaj
							podać odpowiedniego katalogu-należy go również utworzyć. Program
							nie sprawdza czy katalog istnieje; nie możliwe jest ustawienie dostępu do
							kilku katalogów; użytkownik ma dostęp tylko do wyznaczonego katalogu
							i podkatalogów (nie ma dostępu do katalogów nadrzędnych).
						</td>
					</tr>

					<!-- error messages -->
					<xsl:for-each select="inv/rm">
						<tr class="err">
							<td align="center">
								<xsl:text>Nie można usunąć: użytkownik </xsl:text>
								<xsl:value-of select="@user"/>
								<xsl:text>, katalog: </xsl:text>
								<xsl:value-of select="@dir"/>
							</td>
						</tr>
					</xsl:for-each>

					<xsl:for-each select="inv/dir">
						<tr class="err">
							<td align="center">
								<xsl:text>Nieprawidłowy katalog: użytkownik </xsl:text>
								<xsl:value-of select="@user"/>
								<xsl:text>, katalog: </xsl:text>
								<xsl:value-of select="@dir"/>
							</td>
						</tr>
					</xsl:for-each>

					<xsl:for-each select="inv/dirchars">
						<tr class="err">
							<td align="center">
								<xsl:text>Katalog zawierał nieprawidłowe znaki: użytkownik </xsl:text>
								<xsl:value-of select="@user"/>
								<xsl:text>, katalog: </xsl:text>
								<xsl:value-of select="@dir"/>
							</td>
						</tr>
					</xsl:for-each>

					<xsl:for-each select="inv/rep">
						<tr class="err">
							<td align="center">
								<xsl:text>Nie można zamienić: użytkownik </xsl:text>
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
