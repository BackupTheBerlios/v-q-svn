<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="no"/>

<xsl:template match="vquserrm">
<xsl:call-template name="begin"/>
<form method="post">
<xsl:choose>
<xsl:when test="removed">
    U�ytkownik zosta� usuni�ty.
</xsl:when>
<xsl:otherwise>

    <table border="0" width="500">
<xsl:if test="inv">
    <tr><td colspan="2">Nieprawdi�owy adres e-mail lub has�o.</td></tr>
</xsl:if>
    <input type="hidden" name="step" value="0"/>
    <tr><td nowrap="">Adres E-Mail:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=email value="</xsl:text>
        <xsl:value-of select="email/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Has�o:</td>
    <td><input type="password" name="pass"/></td></tr>
    <tr><td colspan="2">
    Uwaga: skasowanie konta oznacza usuni�cie wszelkich danych dotycz�cych konta, oraz
    wiadomo�ci znajduj�cych si� na serwerze. Nie istnieje mo�liwo�� odzyskania danych.
    </td></tr>
    <tr><td nowrap="" colspan="2"><input type="submit" value="Usu�"/></td></tr>
    </table>
</xsl:otherwise>
</xsl:choose>
</form>
<xsl:call-template name="end"/>
</xsl:template> 

<xsl:template match="exception">
<xsl:call-template name="begin"/>

Nast�pi� b��d. Spr�buj ponownie za kilka minut, je�eli sytuacja si�
powt�rzy skontaktuj si� z administratorem.

<xsl:call-template name="end"/>
</xsl:template>

<xsl:template name="begin">
<xsl:text disable-output-escaping="yes">Pragma: no-cache
Content-Type: text/html; charset=UTF-8

&lt;!--
Copyright (c) 2003 Pawel Niewiadomski
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
&lt;title&gt;..:: vq ::..&lt;/title&gt;
&lt;/head&gt;
&lt;body bgcolor="white" color="black"&gt;
</xsl:text>
</xsl:template>

<xsl:template name="end">
<xsl:text disable-output-escaping="yes">
&lt;/body&gt;
&lt;/html&gt;
&lt;/text&gt;
</xsl:text>
</xsl:template>

</xsl:stylesheet>
