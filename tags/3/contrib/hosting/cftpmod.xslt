<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/ftp">U�ytkownicy FTP</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/ftp">
Tutaj mo�esz przyzna� dost�p u�ytkownikom do wyszczeg�lnionych cz�ci
serwisu WWW.
</xsl:template>

<xsl:template match="vqwww/mod/ftp">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie mo�na pobra� aktualnej listy.</td></tr>
</xsl:when>
<xsl:otherwise>

<tr><td colspan="2">
Uwaga: aby umo�liwi� u�ytkownikowi dost�p do strony nie wystarczy tutaj
poda� odpowiedniego katalogu-nale�y go r�wnie� utworzy�. Program
nie sprawdza czy katalog istnieje; nie mo�liwe jest ustawienie dost�pu do
kilku katalog�w; u�ytkownik ma dost�p tylko do wyznaczonego katalogu
i podkatalog�w (nie ma dost�pu do katalog�w nadrz�dnych).
</td></tr>

<!-- error messages -->
<xsl:for-each select="inv/rm">
<tr><td colspan="2">Nie mo�na usun��: u�ytkownik <xsl:value-of
select="@user"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/dir">
<tr><td colspan="2">Nieprawid�owy katalog: u�ytkownik <xsl:value-of
select="@user"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/dirchars">
<tr><td colspan="2">Katalog zawiera� nieprawid�owe znaki: u�ytkownik <xsl:value-of
select="@user"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/rep">
<tr><td colspan="2">Nie mo�na zamieni�: u�ytkownik <xsl:value-of
select="@user"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>


<!-- input form -->
<tr><th>U�ytkownik</th><th>Katalog FTP</th></tr>
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
<tr><td colspan="2"><input type="submit" value="Zmie�"/></td></tr>
<input type="hidden" name="id" value="ftp"/>
<input type="hidden" name="conf" value="1"/>

</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
