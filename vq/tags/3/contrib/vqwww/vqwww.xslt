<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="no"/>
<xsl:param name="uri"/>

<!-- redirection -->
<xsl:template match="redirme">Status: 302 Redirected.
<xsl:text>Location: </xsl:text>
<xsl:value-of select="$uri"/>
<!-- <xsl:text>?uniq=</xsl:text>
<xsl:value-of select="$uniq"/> -->
<xsl:value-of select="text()" disable-output-escaping="yes"/>
Content-Type: text/html

</xsl:template>

<!-- body -->
<xsl:template name="vqwww/body">
<xsl:call-template name="begin"/>

<xsl:choose>
<!-- LOGIN FORM -->
<xsl:when test="log">
<form method="post">
<table width="100%">
<xsl:if test="./log/inv/email/@empty"><tr><td colspan="2">Musisz podać e-mail.</td></tr></xsl:if>
<xsl:if test="./log/inv/email/@syntax"><tr><td colspan="2">Nieprawidłowy adres e-mail.</td></tr></xsl:if>
<xsl:if test="./log/inv/email/@dirty"><tr><td colspan="2">E-mail zawierał nieprawidłowe znaki zostały one zamienione.</td></tr></xsl:if>
<xsl:if test="./log/inv/pass/@empty"><tr><td colspan="2">Musisz podać hasło.</td></tr></xsl:if>
<xsl:if test="./log/inv/pass/@dirty"><tr><td colspan="2">Hasło zawiera nieprawidłowe znaki.</td></tr></xsl:if>
<xsl:if test="./log/inv/auth"><tr><td colspan="2">Autoryzacja nie powiodła się.</td></tr></xsl:if>
<xsl:if test="./log/sto"><tr><td colspan="2">Sesja wygasła proszę się ponownie zalogować.</td></tr></xsl:if>
<tr><td>E-Mail:</td>
<td>
<xsl:text disable-output-escaping="yes">&lt;input type="text" name="email" value="</xsl:text>
<xsl:value-of select="./log/email/@val"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
</td></tr>
<tr><td>Hasło:</td>
<td>
<input type="password" name="pass"/>
</td></tr>
<tr><td colspan="2"><input type="submit" value="Zaloguj"/></td></tr>
</table>
<input type="hidden" name="login" value="1"/>
</form>
</xsl:when>

<!-- -->
<xsl:otherwise>
<table width="100%">
<tr><td nowrap="">
<xsl:apply-templates select="./menu"/>
</td></tr>
<tr><td>
<xsl:apply-templates select="./mod"/>
</td></tr>
</table>
<!-- -->
</xsl:otherwise>
</xsl:choose>
<xsl:call-template name="end"/>
</xsl:template> 

<xsl:template match="vqwww/menu">
<xsl:for-each select="./mod">
<a>
<xsl:attribute name="href">
<xsl:value-of select="$uri"/><xsl:text>?id=</xsl:text><xsl:value-of select="@id"/>
</xsl:attribute>
<xsl:apply-templates/>
</a>
<xsl:text> </xsl:text>
</xsl:for-each>
</xsl:template>

<!-- -->
<xsl:template match="vqwww">
<xsl:for-each select="./cookie">
<xsl:value-of select="text()"/><xsl:text>
</xsl:text>
</xsl:for-each>
<xsl:choose>
<xsl:when test="redirme">
<xsl:apply-templates select="redirme"/>
</xsl:when>
<xsl:otherwise>
<xsl:call-template name="vqwww/body"/>
</xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="exception">
<xsl:call-template name="begin"/>

<xsl:choose>
<xsl:when test="@unexpected">
Nastąpił niezdefiniowany błąd.
</xsl:when>
<xsl:otherwise>
Błąd: <xsl:value-of select="text()"/>
</xsl:otherwise>
</xsl:choose>

<xsl:call-template name="end"/>
</xsl:template>

<xsl:template name="begin">
<xsl:text disable-output-escaping="yes">Pragma: no-cache
Content-Type: text/html; charset=UTF-8

&lt;!--
Copyright (c) 2002 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
--&gt;
&lt;html&gt;
&lt;head&gt;
&lt;title&gt;..:: vqwww ::..&lt;/title&gt;
&lt;/head&gt;
&lt;body bgcolor="white" color="black"&gt;
</xsl:text>
</xsl:template>

<xsl:template name="end">
<xsl:text disable-output-escaping="yes">
&lt;/body&gt;
&lt;/html&gt;
</xsl:text>
</xsl:template>

<xsl:template match="menu/mod/out">Koniec</xsl:template>

<xsl:template match="menu/mod/conf">Konfiguracja</xsl:template>

<xsl:template match="vqwww/mod/conf">
<table width="100%" border="0">
<xsl:for-each select="./mod">
<tr><td>
<a>
<xsl:attribute name="href">
<xsl:value-of select="$uri"/>
<xsl:text disable-output-escaping="yes">?conf=&amp;id=</xsl:text>
<xsl:value-of select="@id"/>
</xsl:attribute>
<xsl:apply-templates select="*[1]"/>
</a>
</td></tr>
<tr><td>
<xsl:apply-templates select=".//desc"/>
</td></tr>
</xsl:for-each>
</table>
</xsl:template>

<xsl:include href="file://cpassmod.xslt"/>
<xsl:include href="file://credirmod.xslt"/>
<xsl:include href="file://cautorespmod.xslt"/>

</xsl:stylesheet>
