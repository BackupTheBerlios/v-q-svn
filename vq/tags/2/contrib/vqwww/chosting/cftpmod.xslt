<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/ftp">Użytkownicy FTP</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/ftp">
Tutaj możesz przyznać dostęp użytkownikom do wyszczególnionych części
serwisu WWW.
</xsl:template>

<xsl:template match="vqwww/mod/ftp">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie można pobrać aktualnej listy.</td></tr>
</xsl:when>
<xsl:otherwise>

<tr><td colspan="2">
Uwaga: aby umożliwić użytkownikowi dostęp do strony nie wystarczy tutaj
podać odpowiedniego katalogu-należy go również utworzyć. Program
nie sprawdza czy katalog istnieje; nie możliwe jest ustawienie dostępu do
kilku katalogów; użytkownik ma dostęp tylko do wyznaczonego katalogu
i podkatalogów (nie ma dostępu do katalogów nadrzędnych).
</td></tr>

<!-- error messages -->
<xsl:for-each select="inv/rm">
<tr><td colspan="2">Nie można usunąć: użytkownik <xsl:value-of
select="@user"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/dir">
<tr><td colspan="2">Nieprawidłowy katalog: użytkownik <xsl:value-of
select="@user"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/dirchars">
<tr><td colspan="2">Katalog zawierał nieprawidłowe znaki: użytkownik <xsl:value-of
select="@user"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/rep">
<tr><td colspan="2">Nie można zamienić: użytkownik <xsl:value-of
select="@user"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>


<!-- input form -->
<tr><th>Użytkownik</th><th>Katalog FTP</th></tr>
<xsl:for-each select="item">
<tr><td><xsl:value-of select="@user"/></td><td>
<xsl:text disable-output-escaping="yes">&lt;input name="item</xsl:text>
<xsl:value-of select="position()"/>
<xsl:text disable-output-escaping="yes">" value="</xsl:text>
<xsl:value-of select="@val"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
<xsl:text disable-output-escaping="yes">&lt;input type=hidden name="itemid</xsl:text>
<xsl:value-of select="position()"/>
<xsl:text disable-output-escaping="yes">" value="</xsl:text>
<xsl:value-of select="@id"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
<xsl:text disable-output-escaping="yes">&lt;input type=hidden name="itemuser</xsl:text>
<xsl:value-of select="position()"/>
<xsl:text disable-output-escaping="yes">" value="</xsl:text>
<xsl:value-of select="@user"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
</td></tr>
</xsl:for-each>
<tr><td colspan="2"><input type="submit" value="Zmień"/></td></tr>
<input type="hidden" name="id" value="ftp"/>
<input type="hidden" name="conf" value="1"/>

</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
