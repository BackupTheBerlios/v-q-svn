<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="no"/>

<xsl:template match="vqrescue">
<xsl:call-template name="begin"/>
<form method="post">
<xsl:choose>
<xsl:when test="step2">
    Twoje hasło to: <xsl:value-of select="step2/@pass"/>.
</xsl:when>
<xsl:when test="step1">

    <table border="0" width="500">
    <xsl:if test="step1/matchno">
    <tr><td colspan="2">Dane się nie zgadzają.</td></tr>
    </xsl:if>
    <xsl:text disable-output-escaping="yes">&lt;input type=hidden name=email value="</xsl:text>
    <xsl:value-of select="step1/email/@val"/>
    <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    <!-- -->
    <tr><td nowrap="">Pytanie ratunkowe:</td>
    <td><xsl:value-of select="step1/re_que/@val"/></td></tr>
    <tr><td nowrap="">Odpowiedź:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=re_ans value="</xsl:text>
        <xsl:value-of select="step1/re_ans/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2"><hr/></td></tr>
    <tr><td nowrap="">Kraj:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=country value="</xsl:text>
        <xsl:value-of select="step1/country/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Województwo:</td>
    <td><select name="area"><xsl:for-each select="step1/areas/area">
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
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=city value="</xsl:text>
        <xsl:value-of select="step1/city/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    <tr><td nowrap="">Płeć:</td>
    <td><select name="sex"><xsl:for-each select="step1/sexes/sex">
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
    <tr><td nowrap="">Data urodzenia:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=birthday value="</xsl:text>
        <xsl:value-of select="step1/birthday/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Wykształcenie:</td>
    <td><select name="edu"><xsl:for-each select="step1/edu/lev">
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
    <tr><td nowrap="">Branża:</td>
    <td><select name="work"><xsl:for-each select="step1/works/work">
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

    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    
    <tr><td nowrap="" colspan="2"><input type="submit" value="Pokaż hasło"/></td></tr>
    </table>
</xsl:when>
<xsl:otherwise>

    <table border="0" width="500">
<xsl:if test="inv/emailno">
    <tr><td colspan="2">Podaj prawidłowy adres e-mail.</td></tr>
</xsl:if>
    <input type="hidden" name="step" value="0"/>
    <tr><td nowrap="">Adres E-Mail:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=email value="</xsl:text>
        <xsl:value-of select="email/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
 
    <tr><td nowrap="" colspan="2"><input type="submit" value="Dalej >>"/></td></tr>
    </table>
</xsl:otherwise>
</xsl:choose>
</form>
<xsl:call-template name="end"/>
</xsl:template> 

<xsl:template match="vqsignup">
<xsl:call-template name="begin"/>
<form method="post">
<xsl:choose>
<xsl:when test="add/@ok">
<table>
<tr><td colspan="2">
Gratulujemy - użytkownik został dodany.
</td></tr>
<tr>
 <td>Adres e-mail:</td>
 <td><xsl:value-of select="add/user"/>@<xsl:value-of select="add/domain"/></td></tr>
<tr><td>Serwer SMTP:</td>
 <td>smtp.<xsl:value-of select="add/domain"/></td></tr>
<tr><td>Serwer POP3:</td>
 <td>pop3.<xsl:value-of select="add/domain"/></td></tr>
</table>
</xsl:when>
<xsl:otherwise>

    <table border="0" width="500">
<xsl:if test="add/@user_ex">
    <tr><td colspan="2">Użytkownik istnieje, wybierz inną nazwę.</td></tr>
</xsl:if>
<!-- INV -->
<xsl:if test="inv">

<xsl:if test="inv/user/@dirty">
    <tr><td colspan="2">Nazwa użytkownika zawiera nieprawidłowe znaki, zostały one zamienione '_'.</td></tr>
</xsl:if>
<xsl:if test="inv/user/@lo">
    <tr><td colspan="2">Nazwa użytkownika jest za długa.</td></tr>
</xsl:if>
<xsl:if test="inv/user/@sh">
    <tr><td colspan="2">Nazwa użytkownika jest za krótka.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@dirty">
    <tr><td colspan="2">Hasło zawiera nieprawidłowe znaki, zostały one zamienione '_'.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@lo">
    <tr><td colspan="2">Hasło jest za długie.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@sh">
    <tr><td colspan="2">Hasło jest za krótkie.</td></tr>
</xsl:if>
<xsl:if test="inv/pass/@nomatch">
    <tr><td colspan="2">Hasła się nie zgadzają.</td></tr>
</xsl:if>
<xsl:if test="inv/domain">
    <tr><td colspan="2">Nieprawidłowa domena.</td></tr>
</xsl:if>
<xsl:if test="inv/country/@sh">
    <tr><td colspan="2">Nazwa kraju jest za krótka.</td></tr>
</xsl:if>
<xsl:if test="inv/area/@inv">
    <tr><td colspan="2">Nieprawidłowe województwo.</td></tr>
</xsl:if>
<xsl:if test="inv/area/@no">
    <tr><td colspan="2">Wybierz województwo.</td></tr>
</xsl:if>
<xsl:if test="inv/city/@sh">
    <tr><td colspan="2">Nazwa miasta jest za krótka.</td></tr>
</xsl:if>
<xsl:if test="inv/birthday/@inv">
    <tr><td colspan="2">Data urodzenia jest nieprawidłowa.</td></tr>
</xsl:if>
<xsl:if test="inv/ints/@no">
    <tr><td colspan="2">Musisz podać co najmniej jedno zainteresowanie.</td></tr>
</xsl:if>

</xsl:if><!-- INV -->

    <tr><td nowrap="">Nazwa użytkownika:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=user value="</xsl:text>
        <xsl:value-of select="user/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2">
    Minimalna długość: <xsl:value-of select="user/@minl"/>, maksymalna: 
    <xsl:value-of select="user/@maxl"/>. Dozwolone znaki to: 
    <xsl:value-of select="user/@allowed"/>.
    </td></tr>
    <tr><td nowrap="">Domena:</td>
    <td><select name="domain"><xsl:for-each select="domains/domain">
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
    <tr><td nowrap="">Hasło:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=password name=pass value="</xsl:text>
        <xsl:value-of select="pass/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2">
    Minimalna długość: <xsl:value-of select="pass/@minl"/>, maksymalna: 
    <xsl:value-of select="pass/@maxl"/>. Dozwolone znaki to: 
    <xsl:value-of select="pass/@allowed"/>.
    </td></tr>
    <tr><td nowrap="">Kopia hasła:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=password name=pass1 value="</xsl:text>
        <xsl:value-of select="pass1/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    <tr><td nowrap="">Pytanie ratunkowe:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=re_que value="</xsl:text>
        <xsl:value-of select="re_que/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td nowrap="">Odpowiedź:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=re_ans value="</xsl:text>
        <xsl:value-of select="re_ans/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2"><hr/></td></tr>
    <tr><td nowrap="">Kraj:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=country value="</xsl:text>
        <xsl:value-of select="country/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2">
    Minimalna długość: <xsl:value-of select="country/@minl"/>.
    </td></tr>
    <tr><td nowrap="">Województwo:</td>
    <td><select name="area"><xsl:for-each select="areas/area">
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
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=city value="</xsl:text>
        <xsl:value-of select="city/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2">
    Minimalna długość: <xsl:value-of select="city/@minl"/>.
    </td></tr>
    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    <tr><td nowrap="">Płeć:</td>
    <td><select name="sex"><xsl:for-each select="sexes/sex">
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
    <tr><td nowrap="">Data urodzenia:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=birthday value="</xsl:text>
        <xsl:value-of select="birthday/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2">
    Data w formacie YYYY-MM-DD (rok-miesiąc-dzień).
    </td></tr>
    <tr><td nowrap="">Wykształcenie:</td>
    <td><select name="edu"><xsl:for-each select="edu/lev">
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
    <tr><td nowrap="">Branża:</td>
    <td><select name="work"><xsl:for-each select="works/work">
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
    <tr><td colspan="2">Zainteresowania:</td></tr>
    <xsl:for-each select="ints/int">
    <xsl:if test="position() mod 2 = 1">
    <xsl:text disable-output-escaping="yes">&lt;tr&gt;</xsl:text>
    </xsl:if>
    <td nowrap="">
        <xsl:text disable-output-escaping="yes">&lt;input type=checkbox name="int</xsl:text>
        <xsl:value-of select="position()"/>
        <xsl:text>" value="</xsl:text>
        <xsl:value-of select="@id"/>
        <xsl:text disable-output-escaping="yes">"</xsl:text>
        <xsl:if test="@sel">
            <xsl:text> checked </xsl:text>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        <xsl:value-of select="@name"/>
    </td>
    <xsl:if test="position() mod 2 = 0">
    <xsl:text disable-output-escaping="yes">&lt;/tr&gt;</xsl:text>
    </xsl:if>
    </xsl:for-each>

    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    
    <tr><td nowrap="" colspan="2"><input type="submit" value="Załóż konto"/></td></tr>
    </table>

</xsl:otherwise>
</xsl:choose>
</form>
<xsl:call-template name="end"/>
</xsl:template> 

<xsl:template match="exception">
<xsl:call-template name="begin"/>

Nastąpił błąd. Spróbuj ponownie za kilka minut, jeżeli sytuacja się
powtórzy skontaktuj się z administratorem.

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
&lt;title&gt;..:: vqsignup ::..&lt;/title&gt;
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
