<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="no"/>

<xsl:template match="vqsignup">
<xsl:text disable-output-escaping="yes">Content-Type: text/html

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
--&gt;</xsl:text>
<html>
<head>
<title>..:: vqsignup ::..</title>
</head>
<body background="white" color="black">
<form method="post">
<xsl:choose>
<xsl:when test="add/@ok">
    Gratulujemy u�ytkownik zosta� dodany.
</xsl:when>
<xsl:otherwise>

    <table border="0" width="500">
<xsl:if test="add/@user_ex">
    <tr><td colspan="2">U�ytkownik istnieje, wybierz inn� nazw�.</td></tr>
</xsl:if>
<xsl:if test="inv/user/@dirty">
    <tr><td colspan="2">Nazwa u�ytkownika zawiera nieprawid�owe znaki, zosta�y one zamienione '_'.</td></tr>
</xsl:if>
<xsl:if test="inv/user/@toolong">
    <tr><td colspan="2">Nazwa u�ytkownika jest za d�uga.</td></tr>
</xsl:if>
<xsl:if test="inv/user/@tooshort">
    <tr><td colspan="2">Nazwa u�ytkownika jest za kr�tka.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@dirty">
    <tr><td colspan="2">Has�o zawiera nieprawid�owe znaki, zosta�y one zamienione '_'.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@toolong">
    <tr><td colspan="2">Has�o jest za d�ugie.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@tooshort">
    <tr><td colspan="2">Has�o jest za kr�tkie.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@nomatch">
    <tr><td colspan="2">Has�a si� nie zgadzaj�.</td></tr>
</xsl:if>
<xsl:if test="inv/domain">
    <tr><td colspan="2">Nieprawid�owa domena.</td></tr>
</xsl:if>
    <tr><td nowrap="">Nazwa u�ytkownika:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=user value="</xsl:text>
        <xsl:value-of select="user/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Domena:</td>
    <td><select name="domain"><xsl:for-each select="domains/domain">
        <xsl:text disable-output-escaping="yes">&lt;option value="</xsl:text>
        <xsl:value-of select="@val"/>
        <xsl:text disable-output-escaping="yes">" </xsl:text>
        <xsl:if test="@sel">
        <xsl:text disable-output-escaping="yes"> selected</xsl:text>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        <xsl:value-of select="@val"/>
        <xsl:text disable-output-escaping="yes">&lt;/option&gt;</xsl:text>
    </xsl:for-each>
    </select></td></tr>
    <tr><td nowrap="">Has�o:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=password name=pass value="</xsl:text>
        <xsl:value-of select="pass/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Kopia has�a:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=password name=pass1 value="</xsl:text>
        <xsl:value-of select="pass1/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="" colspan="2"><input type="submit" value="Za�� konto"/></td></tr>
    </table>

</xsl:otherwise>
</xsl:choose>
</form>
</body>
</html>
</xsl:template> 

<xsl:template match="exception">
<xsl:choose>
<xsl:when test="@unexpected">
Nast�pi� niezdefiniowany b��d.
</xsl:when>
<xsl:otherwise>
B��d: <xsl:value-of select="text()"/>
</xsl:otherwise>
</xsl:choose>
</xsl:template>

</xsl:stylesheet>
