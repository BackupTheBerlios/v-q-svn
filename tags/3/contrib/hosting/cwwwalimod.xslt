<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/wwwali">Aliasy WWW</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/wwwali">
Pozwala zarządzać poddomenami. Tutaj możesz skonfigurować adresy
www.domena, ftp.domena.
</xsl:template>

<xsl:template match="vqwww/mod/wwwali">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie można odczytać aktualnych ustawień.</td></tr>
</xsl:when>
<xsl:otherwise>

<tr><td colspan="2">
Uwaga: dla każdej z domen prefixy ustanawia się osobno. Program nie sprawdza
czy podany katalog istnieje. Aby usunąć prefix należy usunąć
jego nazwę i zapisać zmiany.
</td></tr>

<tr><td colspan="2">
Domeny: <xsl:for-each select="dom">
<xsl:if test="not(@sel)">
<xsl:text disable-output-escaping="yes">&lt;a href="</xsl:text>
<xsl:value-of select="$uri"/>
<xsl:text disable-output-escaping="yes">?conf=1&amp;id=wwwali&amp;domid=</xsl:text>
<xsl:value-of select="@id"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
</xsl:if>
<xsl:value-of select="@name"/>
<xsl:if test="not(@sel)">
<xsl:text disable-output-escaping="yes">&lt;/A&gt;</xsl:text>
</xsl:if>
<xsl:text> </xsl:text>
</xsl:for-each>
</td></tr>

<!-- error messages -->
<xsl:for-each select="inv/rm">
<tr><td colspan="2">Nie można usunąć: prefix: <xsl:value-of
select="@prefix"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/dir">
<tr><td colspan="2">Nieprawidłowy katalog: prefix <xsl:value-of
select="@prefix"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/dirchars">
<tr><td colspan="2">Katalog zawierał nieprawidłowe znaki: prefix <xsl:value-of
select="@prefix"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/prefix">
<tr><td colspan="2">Nieprawidłowy prefix: prefix <xsl:value-of
select="@prefix"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/prefixchars">
<tr><td colspan="2">Prefix zawierał nieprawidłowe znaki: prefix <xsl:value-of
select="@prefix"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/rep">
<tr><td colspan="2">Nie można zamienić: prefix: <xsl:value-of
select="@prefix"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/add">
<tr><td colspan="2">Nie można dodać: prefix: <xsl:value-of
select="@prefix"/>, katalog: <xsl:value-of select="@dir"/></td></tr>
</xsl:for-each>


<!-- input form -->
<tr><th>Prefix</th><th>Katalog</th></tr>
<xsl:for-each select="item">
<tr><td>
<xsl:text disable-output-escaping="yes">&lt;input name="itempre</xsl:text>
<xsl:value-of select="position()"/>
<xsl:text disable-output-escaping="yes">" value="</xsl:text>
<xsl:value-of select="@prefix"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
</td><td>
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
</td></tr>
</xsl:for-each>

<tr><td colspan="2"><input type="submit" value="Zmień"/></td></tr>
<input type="hidden" name="id" value="wwwali"/>
<input type="hidden" name="conf" value="1"/>
<xsl:text disable-output-escaping="yes">&lt;input name="domid" type="hidden" value="</xsl:text>
<xsl:value-of select="domid/@id"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>

</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
