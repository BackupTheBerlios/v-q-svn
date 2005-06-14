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
<!-- INV -->
<xsl:if test="inv">

<xsl:if test="inv/user/@dirty">
    <tr><td colspan="2">Nazwa u�ytkownika zawiera nieprawid�owe znaki, zosta�y one zamienione '_'.</td></tr>
</xsl:if>
<xsl:if test="inv/user/@lo">
    <tr><td colspan="2">Nazwa u�ytkownika jest za d�uga.</td></tr>
</xsl:if>
<xsl:if test="inv/user/@sh">
    <tr><td colspan="2">Nazwa u�ytkownika jest za kr�tka.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@dirty">
    <tr><td colspan="2">Has�o zawiera nieprawid�owe znaki, zosta�y one zamienione '_'.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@lo">
    <tr><td colspan="2">Has�o jest za d�ugie.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@sh">
    <tr><td colspan="2">Has�o jest za kr�tkie.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@nomatch">
    <tr><td colspan="2">Has�a si� nie zgadzaj�.</td></tr>
</xsl:if>
<xsl:if test="inv/domain">
    <tr><td colspan="2">Nieprawid�owa domena.</td></tr>
</xsl:if>
<xsl:if test="inv/f_name/@sh">
    <tr><td colspan="2">Nazwa firmy jest za kr�tka.</td></tr>
</xsl:if>
<xsl:if test="inv/f_nip/@inv">
    <tr><td colspan="2">NIP firmy jest nieprawid�owy.</td></tr>
</xsl:if>
<xsl:if test="inv/f_country/@sh">
    <tr><td colspan="2">Nazwa kraju jest za kr�tka.</td></tr>
</xsl:if>
<xsl:if test="inv/area/@inv">
    <tr><td colspan="2">Nieprawid�owe wojew�dztwo.</td></tr>
</xsl:if>
<xsl:if test="inv/area/@no">
    <tr><td colspan="2">Wybierz wojew�dztwo.</td></tr>
</xsl:if>
<xsl:if test="inv/f_city/@sh">
    <tr><td colspan="2">Nazwa miasta jest za kr�tka.</td></tr>
</xsl:if>
<xsl:if test="inv/f_str/@sh">
    <tr><td colspan="2">Nazwa ulicy jest za kr�tka.</td></tr>
</xsl:if>
<xsl:if test="inv/f_house_no/@sh">
    <tr><td colspan="2">Podaj numer budynku.</td></tr>
</xsl:if>
<xsl:if test="inv/f_house_no/@dirty">
    <tr><td colspan="2">Nieprawid�owy numer budynku (tylko cyfry i znaki a-z, A-Z.</td></tr>
</xsl:if>
<xsl:if test="inv/f_app_no/@dirty">
    <tr><td colspan="2">Nieprawid�owy numer mieszkania (tylko cyfry).</td></tr>
</xsl:if>
<xsl:if test="inv/f_pcode/@inv">
    <tr><td colspan="2">Nieprawid�owy kod pocztowy.</td></tr>
</xsl:if>
<xsl:if test="inv/c_name/@sh">
    <tr><td colspan="2">Imi� i nazwisko osoby do kontatku s� za kr�tkie.</td></tr>
</xsl:if>
<xsl:if test="inv/c_email/@empty">
    <tr><td colspan="2">Podaj kontaktowy adres e-mail.</td></tr>
</xsl:if>
<xsl:if test="inv/c_email/@inv">
    <tr><td colspan="2">Kontaktowy adres e-mail jest nieprawid�owy.</td></tr>
</xsl:if>
<xsl:if test="inv/c_phone/@sh">
    <tr><td colspan="2">Numer telefonu kontaktowego jest za kr�tki.</td></tr>
</xsl:if>
<xsl:if test="inv/c_phone/@dirty">
    <tr><td colspan="2">Numer telefonu kontaktowego zawiera nieprawid�owe znaki.</td></tr>
</xsl:if>

</xsl:if><!-- INV -->

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
    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    <tr><td nowrap="">Nazwa firmy (lub imi� i nazwisko):</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=f_name value="</xsl:text>
        <xsl:value-of select="f_name/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">NIP:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=f_nip value="</xsl:text>
        <xsl:value-of select="f_nip/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Kraj:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=f_country value="</xsl:text>
        <xsl:value-of select="f_country/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Wojew�dztwo:</td>
    <td><select name="f_area"><xsl:for-each select="areas/area">
        <xsl:text disable-output-escaping="yes">&lt;option value="</xsl:text>
        <xsl:value-of select="@id"/>
        <xsl:text disable-output-escaping="yes">" </xsl:text>
        <xsl:if test="@sel">
        <xsl:text disable-output-escaping="yes"> selected</xsl:text>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text disable-output-escaping="yes">&lt;/option&gt;</xsl:text>
    </xsl:for-each>
    </select></td></tr>
    <tr><td nowrap="">Miasto:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=f_city value="</xsl:text>
        <xsl:value-of select="f_city/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Ulica:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=f_str value="</xsl:text>
        <xsl:value-of select="f_str/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Nr budynku:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=f_house_no value="</xsl:text>
        <xsl:value-of select="f_house_no/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Nr mieszkania:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=f_app_no value="</xsl:text>
        <xsl:value-of select="f_app_no/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Kod pocztowy:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=f_pcode value="</xsl:text>
        <xsl:value-of select="f_pcode/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    <tr><td nowrap="">Imi� i nazwisko zarz�dzaj�cego:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=c_name value="</xsl:text>
        <xsl:value-of select="c_name/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">E-Mail kontaktowy (na innym serwerze):</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=c_email value="</xsl:text>
        <xsl:value-of select="c_email/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Telefon kontaktowy:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=c_phone value="</xsl:text>
        <xsl:value-of select="c_phone/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    
    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    
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
