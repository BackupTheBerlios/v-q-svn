<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/users">U�ytkownicy</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/users">
Pokazuje list� kont pocztowych zarejestrowanych dla tej domeny, oraz zwi�zane
z nimi informacje, takie jak data wa�no�ci, itd.
</xsl:template>

<!-- OPERATION LINK -->
<xsl:template name="users_opl">
<xsl:param name="desc"/>
<xsl:param name="act"/>
<xsl:text disable-output-escaping="yes">&lt;A HREF="</xsl:text>
<xsl:value-of select="$uri"/>
<xsl:text disable-output-escaping="yes">?conf=1&amp;id=users&amp;uid=</xsl:text>
<xsl:value-of select="@id"/>
<xsl:text disable-output-escaping="yes">&amp;act=</xsl:text>
<xsl:value-of select="$act"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
<xsl:value-of select="$desc"/>
<xsl:text disable-output-escaping="yes">&lt;/A&gt;</xsl:text>
</xsl:template>

<!-- MAIN HTML -->
<xsl:template match="vqwww/mod/users">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="cant/get">
<tr><td>Nie mo�na pobra� aktualnej listy.</td></tr>
</xsl:when>

<!-- VERIFICATION -->
<xsl:when test="ver/rm">
<tr><td colspan="2">
Uwaga: odzyskanie danych usuni�tego u�ytkownika, w tym jego poczty, nie jest
mo�liwe.<br/>
Usun�� u�ytkownika?
</td></tr>
<tr><td>
<xsl:text disable-output-escaping="yes">&lt;A HREF="</xsl:text>
<xsl:value-of select="$uri"/>
<xsl:text disable-output-escaping="yes">?conf=1&amp;id=users"&gt;Nie&lt;/A&gt;</xsl:text>
</td><td>
<xsl:text disable-output-escaping="yes">&lt;A HREF="</xsl:text>
<xsl:value-of select="$uri"/>
<xsl:text disable-output-escaping="yes">?conf=1&amp;id=users&amp;uid=</xsl:text>
<xsl:value-of select="ver/rm/@id"/>
<xsl:text disable-output-escaping="yes">&amp;act=r&amp;ver=1"&gt;Tak&lt;/A&gt;</xsl:text>
</td></tr>
</xsl:when>
<xsl:otherwise>

<!-- error messages -->
<xsl:if test="cant/rm">
<tr><td colspan="2">Nie mo�na usun�� u�ytkownika</td></tr>
</xsl:if>

<!-- input form -->
<tr><th>U�ytkownik</th><th>Data utworzenia</th><th>Data wa�no�ci</th>
    <th>Operacje</th></tr>
<xsl:for-each select="item">
<tr><td><xsl:value-of select="@user"/></td>
<td><xsl:value-of select="@date_crt"/></td>
<td><xsl:value-of select="@date_exp"/></td>
<td>
<xsl:call-template name="users_opl">
<xsl:with-param name="desc">Usu�</xsl:with-param>
<xsl:with-param name="act">r</xsl:with-param>
</xsl:call-template>

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
</td>
</tr>
</xsl:for-each>
</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
