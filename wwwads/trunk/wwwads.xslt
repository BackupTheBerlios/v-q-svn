<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="no"/>

<xsl:param name="uri"/>
<xsl:param name="WWWDIR"/>
<xsl:param name="uniq"/>

<xsl:template match="http/redirme">Status: 302 Redirected.
<xsl:text>Location: </xsl:text>
<xsl:value-of select="$uri"/>
<xsl:text>?uniq=</xsl:text>
<xsl:value-of select="$uniq"/>
<xsl:value-of select="text()" disable-output-escaping="yes"/>
Content-Type: text/html

</xsl:template>

<xsl:template match="wwwads/http"><xsl:apply-templates/><xsl:value-of select="text()"/></xsl:template>

<xsl:template match="wwwads">
<xsl:apply-templates select="http"/>Pragma: no-cache
Content-Type: text/html

<xsl:text disable-output-escaping="yes">&lt;!--
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
<title>..:: WWWADS-conf ::..</title>
</head>
<body background="white" color="black">
    <xsl:apply-templates select="main"/>
    <xsl:apply-templates select="grps"/>
    <xsl:apply-templates select="ads"/>
    <xsl:apply-templates select="exception"/>
</body>
</html>
</xsl:template> 

<!-- COMMON MESSAGES FORM ADS/ADD ADS/CHG -->
<xsl:template match="ads/add|chg/dirtymc">
<tr><td colspan="2"><p><b>Max clicks included invalid chars, they were replaced with "_".</b></p></td></tr>
</xsl:template>
<xsl:template match="ads/add|chg/dirtyms">
<tr><td colspan="2"><p><b>Max shows included invalid chars, they were replaced with "_".</b></p></td></tr>
</xsl:template>
<xsl:template match="ads/add|chg/dirtyttl">
<tr><td colspan="2"><p><b>Time-to-live included invalid chars, they were replaced with "_".</b></p></td></tr>
</xsl:template>
<xsl:template match="ads/add|chg/dirtymisc">
<tr><td colspan="2"><p><b>Misc included invalid chars, they were replaced with "_".</b></p></td></tr>
</xsl:template>
<xsl:template match="ads/add/dirtygrp">
<tr><td colspan="2"><p><b>Group included invalid chars, they were replaced with "_".</b></p></td></tr>
</xsl:template>
<xsl:template match="ads/add|chg/dirtyurl">
<tr><td colspan="2"><p><b>URL included invalid chars, they were replaced with "_".</b></p></td></tr>
</xsl:template>

<xsl:template match="ads/add|chg/nanmc">
<tr><td colspan="2"><p><b>Max cliks isn't unsigned integer.</b></p></td></tr>
</xsl:template>
<xsl:template match="ads/add|chg/nanms">
<tr><td colspan="2"><p><b>Max shows isn't unsigned integer.</b></p></td></tr>
</xsl:template>
<xsl:template match="ads/add|chg/nanttl">
<tr><td colspan="2"><p><b>TTL isn't unsigned integer.</b></p></td></tr>
</xsl:template>

<xsl:template match="ads/add|chg/nourl">
<tr><td colspan="2"><p><b>URL is missing.</b></p></td></tr>
</xsl:template>

<xsl:template match="ads/add">
<table>
<tr><td valign="top"><xsl:call-template name="main"/></td><td valign="top">

<form method="post" enctype="multipart/form-data">
<input type="hidden" value="2" name="what"/>
<input type="hidden" value="2" name="act"/>
<table>
<!-- MESSAGES -->
<xsl:if test="ok">
<tr><td colspan="2"><p><b>Advertisement was added.</b></p></td></tr>
</xsl:if>
<xsl:if test="cant">
<tr><td colspan="2"><p><b>Can't add advertisement.</b></p></td></tr>
</xsl:if>
<xsl:if test="nofile">
<tr><td colspan="2"><p><b>File is missing.</b></p></td></tr>
</xsl:if>
<xsl:if test="grps/cant">
<tr><td colspan="2"><p><b>Can't get group list.</b></p></td></tr>
</xsl:if>
<xsl:if test="nogrp">
<tr><td colspan="2"><p><b>You didn't specify group.</b></p></td></tr>
</xsl:if>

<xsl:apply-templates select=".//dirtymc"/>
<xsl:apply-templates select=".//dirtyms"/>
<xsl:apply-templates select=".//dirtyttl"/>
<xsl:apply-templates select=".//dirtymisc"/>
<xsl:apply-templates select=".//dirtygrp"/>
<xsl:apply-templates select=".//dirtyurl"/>
<xsl:apply-templates select=".//nanmc"/>
<xsl:apply-templates select=".//nanms"/>
<xsl:apply-templates select=".//nanttl"/>
<xsl:apply-templates select=".//nourl"/>
<tr><td>Group</td>
<td><xsl:apply-templates select=".//grps/cant"/><select name="grp">
<xsl:for-each select="grps/grp">
<xsl:sort select="grps/grp/name" data-type="text"/>
<xsl:text disable-output-escaping="yes">&lt;option </xsl:text><xsl:if test="selected">selected</xsl:if><xsl:text disable-output-escaping="yes"> value="</xsl:text><xsl:value-of select="id"/><xsl:text disable-output-escaping="yes">"&gt;</xsl:text><xsl:value-of select="name"/><xsl:text disable-output-escaping="yes">&lt;/option&gt;</xsl:text>
</xsl:for-each>
</select></td></tr>
<tr><td>File:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="file" name="file" value="<xsl:value-of select="file"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>URL:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="url" value="<xsl:value-of select="url"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Max clicks:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="mc" value="<xsl:value-of select="mc"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Max shows:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="ms" value="<xsl:value-of select="ms"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Time-to-live (days):</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="ttl" value="<xsl:value-of select="ttl"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Misc info:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="misc" value="<xsl:value-of select="misc"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td><input type="submit" value="Add"/></td></tr>
</table>
</form>
</td></tr>
</table>
</xsl:template>

<!-- AD'S CHANGE -->
<xsl:template match="ads/chg">
<table>
<tr><td valign="top"><xsl:call-template name="main"/></td><td valign="top">

<xsl:choose>
<xsl:when test="cant">
<p><b>Can't get advertisement's information.</b></p>
</xsl:when>
<xsl:otherwise>
<form method="post">
<input type="hidden" value="2" name="what"/>
<input type="hidden" value="1" name="act"/>
<input type="hidden" value="2" name="op"/>
<xsl:text disable-output-escaping="yes">&lt;input type="hidden" name="val" value="</xsl:text>
<xsl:value-of select="val"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
<xsl:text disable-output-escaping="yes">&lt;input type="hidden" name="grp" value="</xsl:text>
<xsl:value-of select="grp"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
<table>
<!-- MESSAGES -->
<xsl:if test="dirtycc">
<tr><td colspan="2"><p><b>Current number of clicks included invalid chars, they were replaced with "_".</b></p></td></tr>
</xsl:if>
<xsl:if test="dirtycs">
<tr><td colspan="2"><p><b>Current number of shows included invalid chars, they were replaced with "_".</b></p></td></tr>
</xsl:if>
<xsl:if test="nancc">
<tr><td colspan="2"><p><b>Current number of clicks isn't unsigned integer.</b></p></td></tr>
</xsl:if>
<xsl:if test="nancc">
<tr><td colspan="2"><p><b>Current number of shows isn't unsigned integer.</b></p></td></tr>
</xsl:if>
<xsl:if test="cantchg">
<tr><td colspan="2"><p><b>Can't update advertisement's information.</b></p></td></tr>
</xsl:if>
<xsl:if test="ok">
<tr><td colspan="2"><p><b>Advertisement's information was changed.</b></p></td></tr>
</xsl:if>
<xsl:apply-templates select=".//dirtyms"/>
<xsl:apply-templates select=".//dirtymc"/>
<xsl:apply-templates select=".//dirtyttl"/>
<xsl:apply-templates select=".//dirtymisc"/>
<xsl:apply-templates select=".//dirtyurl"/>
<xsl:apply-templates select=".//nanmc"/>
<xsl:apply-templates select=".//nanms"/>
<xsl:apply-templates select=".//nanttl"/>
<xsl:apply-templates select=".//nourl"/>
<tr><td>URL:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="url" value="<xsl:value-of select="url"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Max clicks:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="mc" value="<xsl:value-of select="mc"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Current number of clicks:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="cc" value="<xsl:value-of select="cc"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Max shows:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="ms" value="<xsl:value-of select="ms"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Current number of shows:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="cs" value="<xsl:value-of select="cs"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Time-to-live (days):</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="ttl" value="<xsl:value-of select="ttl"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td>Misc info:</td><td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="misc" value="<xsl:value-of select="misc"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td></tr>
<tr><td><input type="submit" value="Change"/></td></tr>
</table>
</form>
</xsl:otherwise>
</xsl:choose>

</td></tr></table>
</xsl:template>

<!-- REMOVING ADS -->
<xsl:template match="ads/rm">
<table><tr><td valign="top"><xsl:call-template name="main"/></td><td valign="top">

<xsl:choose>
<xsl:when test="cant">
<p><b>Can't delete advertisement.</b></p>
</xsl:when>
<xsl:otherwise>
<table border="0">
<tr><td colspan="2"><p><b>Delete advertisement?</b></p></td></tr>
<tr>
    <td><xsl:text disable-output-escaping="yes">&lt;A href="</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=2&amp;act=1&amp;grp=</xsl:text>
        <xsl:value-of select="grp"/>
        <xsl:text disable-output-escaping="yes">"&gt;No&lt;/a&gt;</xsl:text>
    </td>
    <td><form method="post">
        <input type="hidden" name="what" value="2"/>
        <input type="hidden" name="act" value="1"/>
        <input type="hidden" name="op" value="1"/>
        <xsl:text disable-output-escaping="yes">&lt;input type="hidden" name="val" value="</xsl:text>
        <xsl:value-of select="text()"/>
        <xsl:text disable-output-escaping="yes">"&gt;&lt;input type="hidden" name="grp" value="</xsl:text>
        <xsl:value-of select="grp"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
        <input type="submit" value="Yes"/>
        </form>
    </td>
</tr>
</table>
</xsl:otherwise>
</xsl:choose>

</td></tr></table>
</xsl:template>

<!-- SHOW ADS -->
<xsl:template match="ads/ls">
<table><tr><td valign="top"><xsl:call-template name="main"/></td><td valign="top">

<xsl:choose>
<xsl:when test="cant"><p><b>Can't get data.</b></p></xsl:when>
<xsl:otherwise>
<table>
<tr><td>
    <xsl:text>Select group: </xsl:text>
    <xsl:for-each select="grps/grp">
    <xsl:sort select="grps/grp/name" data-type="text"/>
        <xsl:if test="selected"><xsl:text>>> </xsl:text></xsl:if>
        <xsl:text disable-output-escaping="yes">&lt;A HREF="</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=2&amp;act=1&amp;grp=</xsl:text>
        <xsl:value-of select="id"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
        <xsl:value-of select="name"/>
        <xsl:text disable-output-escaping="yes">&lt;/A&gt; </xsl:text> 
    </xsl:for-each>
</td></tr>
<tr><td width="100%">
<table width="100%">
<xsl:if test="ad">
<tr><th nowrap="">ID</th>
    <th nowrap="">Max clicks</th>
    <th nowrap="">Cur. cl.</th>
    <th nowrap="">Max shows</th>
    <th nowrap="">Cur. sh.</th>
    <th nowrap="">Time to live</th>
    <th nowrap="">Misc</th>
    <th nowrap="">Operation</th>
    <th nowrap="">Banner</th>
</tr>
</xsl:if>
    <xsl:for-each select="ad">
    <xsl:sort select="ad/id" data-type="text"/>
        <tr>
        <td><xsl:value-of select="id"/></td>
        <td><xsl:value-of select="mc"/></td>
        <td><xsl:value-of select="cc"/></td>
        <td><xsl:value-of select="ms"/></td>
        <td><xsl:value-of select="cs"/></td>
        <td><xsl:value-of select="ttl"/></td>
        <td><xsl:value-of select="misc"/></td>
    <td><xsl:text disable-output-escaping="yes">&lt;A HREF=</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=2&amp;act=1&amp;op=1&amp;val=</xsl:text>
        <xsl:value-of select="id"/>
        <xsl:text disable-output-escaping="yes">&amp;grp=</xsl:text>
        <xsl:value-of select="ancestor-or-self::*/grp"/>
        <xsl:text disable-output-escaping="yes">&amp;uniq=</xsl:text>
        <xsl:value-of select="$uniq"/>
        <xsl:text disable-output-escaping="yes">"&gt;Remove&lt;/A&gt;</xsl:text>

        <xsl:text disable-output-escaping="yes"> &lt;A HREF=</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=2&amp;act=1&amp;op=3&amp;val=</xsl:text>
        <xsl:value-of select="id"/>
        <xsl:text disable-output-escaping="yes">&amp;blk=</xsl:text>
        <xsl:choose>
        <xsl:when test="blk">0</xsl:when>
        <xsl:otherwise>1</xsl:otherwise>
        </xsl:choose>
        <xsl:text disable-output-escaping="yes">&amp;uniq=</xsl:text>
        <xsl:value-of select="$uniq"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
        <xsl:choose>
        <xsl:when test="blk">Unblock</xsl:when>
        <xsl:otherwise>Block</xsl:otherwise>
        </xsl:choose>
        <xsl:text disable-output-escaping="yes">&lt;/A&gt;</xsl:text>

        <xsl:text disable-output-escaping="yes"> &lt;A HREF=</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=2&amp;act=1&amp;op=2&amp;val=</xsl:text>
        <xsl:value-of select="id"/>
        <xsl:text disable-output-escaping="yes">&amp;grp=</xsl:text>
        <xsl:value-of select="ancestor-or-self::*/grp"/>
        <xsl:text disable-output-escaping="yes">&gt;Change&lt;/A&gt;</xsl:text>
    </td>
        <td>
            <xsl:text disable-output-escaping="yes">&lt;A HREF="</xsl:text>
            <xsl:value-of select="url"/>
            <xsl:text disable-output-escaping="yes">"&gt;&lt;IMG SRC="</xsl:text>
            <xsl:value-of select="$WWWDIR"/>
            <xsl:value-of select="asisurl"/>
            <xsl:text disable-output-escaping="yes">" BORDER=0&gt;&lt;/A&gt;</xsl:text>
        </td>
        </tr>
    </xsl:for-each>
</table>
</td></tr>
</table>
</xsl:otherwise>
</xsl:choose>

</td></tr></table>
</xsl:template>

<!-- GROUPS REMOVING -->
<xsl:template match="grps/rm">
<table><tr><td valign="top"><xsl:call-template name="main"/></td><td valign="top">

<xsl:choose>
<xsl:when test="cant">
<p><b>Can't delete group.</b></p>
</xsl:when>
<xsl:otherwise>
<table border="0">
<tr><td colspan="2"><p><b>Delete group?</b></p></td></tr>
<tr>
    <td><xsl:text disable-output-escaping="yes">&lt;A href="</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=1&amp;act=1"&gt;No&lt;/a&gt;</xsl:text>
    </td>
    <td><xsl:text disable-output-escaping="yes">&lt;A href="</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=1&amp;act=1&amp;op=1&amp;val1=1&amp;val=</xsl:text>
        <xsl:value-of select="text()"/>
        <xsl:text disable-output-escaping="yes">&amp;uniq=</xsl:text>
        <xsl:value-of select="$uniq"/>
        <xsl:text disable-output-escaping="yes">"&gt;Yes&lt;/a&gt;</xsl:text>
    </td>
</tr>
</table>
</xsl:otherwise>
</xsl:choose>

</td></tr></table>
</xsl:template>

<!-- SHOW GROUPS -->
<xsl:template match="grps/ls">
<table><tr><td valign="top"><xsl:call-template name="main"/></td><td valign="top">

<xsl:choose>
<xsl:when test="cant">
<p><b>Can't get group listing.</b></p>
</xsl:when>
<xsl:when test="no">
<p><b>No groups.</b></p>
</xsl:when>
<xsl:otherwise>
<table border="0">
<tr><th>ID</th><th>Name</th><th>Action</th></tr>
<xsl:for-each select="grp">
<xsl:sort select="name" data-type="text"/>
<tr>
    <td><xsl:value-of select="id"/></td>
    <td><xsl:value-of select="name"/></td>
    <td><xsl:text disable-output-escaping="yes">&lt;A HREF=</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=1&amp;act=1&amp;op=1&amp;val=</xsl:text>
        <xsl:value-of select="id"/>
        <xsl:text disable-output-escaping="yes">&amp;uniq=</xsl:text>
        <xsl:value-of select="$uniq"/>
        <xsl:text disable-output-escaping="yes">&gt;Remove&lt;/A&gt;</xsl:text>

        <xsl:text disable-output-escaping="yes">&lt;!-- &lt;A HREF=</xsl:text>
        <xsl:value-of select="$uri"/>
        <xsl:text disable-output-escaping="yes">?what=1&amp;act=1&amp;op=2&amp;val=</xsl:text>
        <xsl:value-of select="id"/>
        <xsl:text disable-output-escaping="yes">&gt;Change&lt;/A&gt; --&gt;</xsl:text>
    </td>
</tr>
</xsl:for-each>
</table>
</xsl:otherwise>
</xsl:choose>

</td></tr></table>
</xsl:template>

<!-- ADD GROUP -->
<xsl:template match="grps/add">
<table><tr><td valign="top"><xsl:call-template name="main"/></td><td valign="top">

<form method="post">
<input type="hidden" value="1" name="what"/>
<input type="hidden" value="2" name="act"/>
<table border="0">

<!-- Messages -->
<xsl:if test="cant">
<tr><td colspan="2"><p><b>Can't add group, sorry.</b></p></td></tr>
</xsl:if>

<xsl:if test="inv/@dirty">
<tr><td colspan="2"><p><b>Group name included invalid chars, they were replaced with '_'.</b></p></td></tr>
</xsl:if>

<xsl:if test="inv/@len">
<tr><td colspan="2"><p><b>Group name has is empty or has invalid length.</b></p></td></tr>
</xsl:if>

<xsl:if test="ok">
<tr><td colspan="2"><p><b>Group was added.</b></p></td></tr>
</xsl:if>

<tr>
    <td>Name:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>input type="text" name="name" value="<xsl:value-of select="name"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td>
    <td><input type="submit" value="Add"/></td>
</tr>
</table>
</form>

</td></tr></table>
</xsl:template>

<!-- MAIN PART -->
<xsl:template match="wwwads/main" name="main">
<table border="0">
<tr><th colspan="2">Groups</th></tr>
<tr>
<td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>a href="<xsl:value-of select="$uri"/><xsl:text disable-output-escaping="yes">?what=1&amp;act=1"&gt;</xsl:text>Show<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/a<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td>
<td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>a href="<xsl:value-of select="$uri"/><xsl:text disable-output-escaping="yes">?what=1&amp;act=2"&gt;</xsl:text>Add<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/a<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td>
</tr>
<tr><th colspan="2">Ads</th></tr>
<tr>
<td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>a href="<xsl:value-of select="$uri"/><xsl:text disable-output-escaping="yes">?what=2&amp;act=1"&gt;</xsl:text>Show<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/a<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td>
<td><xsl:text disable-output-escaping="yes">&lt;</xsl:text>a href="<xsl:value-of select="$uri"/><xsl:text disable-output-escaping="yes">?what=2&amp;act=2"&gt;</xsl:text>Add<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/a<xsl:text disable-output-escaping="yes">&gt;</xsl:text></td>
</tr>
</table>
</xsl:template>

<xsl:template match="wwwads//link">
<A HREF="{href}"><xsl:value-of select="text()"/></A>
</xsl:template>

<xsl:template match="wwwads//meh">
<xsl:text disable-output-escaping="yes">&lt;A href="</xsl:text>
<xsl:value-of select="$uri"/>?<xsl:value-of select="qs"/>"&gt;
<xsl:value-of select="text()"/>&lt;/a>
</xsl:template>

<xsl:template match="exception">
<xsl:choose>
<xsl:when test="@unexpected">
Nastąpił niezdefiniowany błąd.
</xsl:when>
<xsl:otherwise>
Błąd: <xsl:value-of select="text()"/>
</xsl:otherwise>
</xsl:choose>
</xsl:template>

</xsl:stylesheet>
