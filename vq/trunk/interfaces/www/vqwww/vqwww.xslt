<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="no"/>

<xsl:param name="REQUEST_URI"/>
<xsl:param name="SERVER_NAME"/>
<xsl:param name="LOCALE" select="'pl_PL'"/>
<xsl:param name="HTTPS" select="''"/>

<xsl:variable name="msgs" select="document(concat('file://vqwww.',$LOCALE,'.xml'))/msgs"/>

<!-- redirection -->
<xsl:template match="redirme">
	<xsl:text>Status: 302 Redirected.</xsl:text>
	<xsl:text disable-output-escaping="yes">&#10;</xsl:text>
	<xsl:text>Location: </xsl:text>
	<xsl:value-of select="$REQUEST_URI"/>
	<xsl:value-of select="text()" disable-output-escaping="yes"/>
	<xsl:text disable-output-escaping="yes">&#10;</xsl:text>
	<xsl:text>Content-Type: text/html</xsl:text>
	<xsl:text disable-output-escaping="yes">&#10;&#10;</xsl:text>
</xsl:template>

<!-- body -->
<xsl:template name="vqwww/body">
	<xsl:call-template name="begin"/>

	<xsl:choose>
		<!-- LOGIN FORM -->
		<xsl:when test="log">
			<xsl:apply-templates select="log"/>
		</xsl:when>
		<xsl:otherwise>
			<table width="100%">
				<tr class="menu">
					<td nowrap="">
						<xsl:apply-templates select="menu"/>
					</td>
				</tr>
				<tr>
					<td>
						<xsl:apply-templates select="exception"/>
						<xsl:apply-templates select="mod"/>
					</td>
				</tr>
			</table>
		</xsl:otherwise>
	</xsl:choose>

	<xsl:call-template name="end"/>
</xsl:template> 

<!-- LOGIN FORM -->
<xsl:template match="vqwww/log">
	<form method="post">
		<table align="center">
			<tr class="first">
				<td colspan="2" align="center">
					<b>Virtual Qmail via WWW</b>
				</td>
			</tr>
			<xsl:if test="inv">
				<xsl:if test="inv/email/@empty">
					<tr>
						<td colspan="2" class="err">
							<xsl:value-of select="$msgs/log/m[@n='inv/email/@empty']"/>
						</td>
					</tr>
				</xsl:if>
				<xsl:if test="inv/email/@syntax">
					<tr>
						<td colspan="2" class="err">
							<xsl:value-of select="$msgs/log/m[@n='inv/email/@syntax']"/>
						</td>
					</tr>
				</xsl:if>
				<xsl:if test="inv/email/@dirty">
					<tr>
						<td colspan="2" class="err">
							<xsl:value-of select="$msgs/log/m[@n='inv/email/@dirty']"/>
						</td>
					</tr>
				</xsl:if>
				<xsl:if test="inv/pass/@empty">
					<tr>
						<td colspan="2" class="err">
							<xsl:value-of select="$msgs/log/m[@n='inv/pass/@empty']"/>
						</td>
					</tr>
				</xsl:if>
				<xsl:if test="inv/pass/@dirty">
					<tr>
						<td colspan="2" class="err">
							<xsl:value-of select="$msgs/log/m[@n='inv/pass/@dirty']"/>
						</td>
					</tr>
				</xsl:if>
				<xsl:if test="inv/auth">
					<tr>
						<td colspan="2" class="err">
							<xsl:value-of select="$msgs/log/m[@n='inv/auth']"/>
						</td>
					</tr>
				</xsl:if>
			</xsl:if>
			<xsl:if test="sto">
				<tr>
					<td colspan="2" class="err">
						<xsl:value-of select="$msgs/log/m[@n='sto']"/>
					</td>
				</tr>
			</xsl:if>
	
			<tr>
				<td>
					<xsl:value-of select="$msgs/log/m[@n='locale']"/>
				</td>
				<td>
					<select name="locale" onchange="document.forms[0].submit()">
						<xsl:for-each select="locale">
							<option>
								<xsl:attribute name="value">
									<xsl:value-of select="@code"/>
								</xsl:attribute>
								<xsl:if test="@sel">
									<xsl:attribute name="selected"/>
								</xsl:if>
								<xsl:value-of select="@name"/>
							</option>
						</xsl:for-each>
					</select>
				</td>
			</tr>
	
			<tr>
				<td>
					<xsl:value-of select="$msgs/log/m[@n='email']"/>
				</td>
				<td>
					<input type="text" name="email">
						<xsl:attribute name="value">
							<xsl:value-of select="email/@val"/>
						</xsl:attribute>
					</input>
				</td>
			</tr>
			<tr>
				<td>
					<xsl:value-of select="$msgs/log/m[@n='pass']"/>
				</td>
				<td>
					<input type="password" name="pass"/>
				</td>
			</tr>
			<tr>
				<td colspan="2" align="center">
					<input type="submit" class="button">
						<xsl:attribute name="value">
							<xsl:value-of select="$msgs/log/m[@n='submit']"/>
						</xsl:attribute>
					</input>
				</td>
			</tr>
		</table>
		<input type="hidden" name="login" value="1"/>
		<input type="hidden" name="locale">
			<xsl:attribute name="value">
				<xsl:value-of select="$LOCALE"/>
			</xsl:attribute>
		</input>
	</form>
</xsl:template>

<!-- DUMP MENU -->
<xsl:template match="vqwww/menu">
	<xsl:for-each select="./mod">
		<a class="menu">
			<xsl:attribute name="href">
				<xsl:value-of select="$REQUEST_URI"/>
				<xsl:text>?id=</xsl:text>
				<xsl:value-of select="@id"/>
			</xsl:attribute>

			<xsl:text disable-output-escaping="yes">[&amp;nbsp;</xsl:text> 
			<xsl:apply-templates/>
			<xsl:text disable-output-escaping="yes">&amp;nbsp;]</xsl:text>
		</a>
		<xsl:text> </xsl:text>
	</xsl:for-each>
</xsl:template>

<!-- -->
<xsl:template match="vqwww">
	<xsl:for-each select="./cookie">
		<xsl:value-of select="text()"/>
		<xsl:text disable-output-escaping="yes">&#10;</xsl:text>
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

<xsl:template match="vqwww//exception" name="exception">
	<table class="err" align="center">
		<tr>
			<td>
				<xsl:value-of select="$msgs/exception/m[@n='desc']"/>
			</td>
		</tr>
		<tr>
			<td>
				<xsl:value-of select="$msgs/exception/m[@n='msg']"/><br/>
			</td>
		</tr>
		<xsl:choose>
			<xsl:when test="@unexpected">
				<tr>
					<td>
						<xsl:value-of select="$msgs/exception/m[@n='@unexpected']"/>
					</td>
				</tr>
			</xsl:when>
			<xsl:otherwise>
				<tr>
					<td>
						<xsl:value-of select="text()"/>
					</td>
				</tr>
			</xsl:otherwise>
		</xsl:choose>
	</table>
</xsl:template>

<xsl:template match="/exception">
	<xsl:call-template name="begin"/>
	
	<xsl:call-template name="exception"/>

	<xsl:call-template name="end"/>
</xsl:template>

<xsl:template name="begin">
<xsl:text disable-output-escaping="yes">Pragma: no-cache
Content-Type: text/html; charset=UTF-8

&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
	"http://www.w3c.org/TR/html4/loose.dtd"&gt;
&lt;!--
Copyright (c) 2002,2003 Pawel Niewiadomski
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
&lt;style type="text/css"&gt;

a:link { 
	color: #004DEA; 
	text-decoration: underline
}

a:visited { 
	color: #003399; 
	text-decoration: underline
}

a:hover { 
	color: #FF6600; 
	text-decoration: none
}

.first,.menu {
	background: #C8E097; 
}

input,textarea,select {
	border-style: dotted;
	background: #F0F0F0;
}

.button {
	border: dotted #C8E097;
	background: #F0F0F0;
}

.sec,.conf {
	background: #F0F0F0;
}

.err {
	font-weight: bold;
}

.desc {
	font-style: italic;
}

TH {
	text-align: left;
	background: #C8E097;
}

BODY {
	font-family: arial, sans-serif;
	font-size: 10pt;
	background: #FFFFFF;
	scrollbar-arrow-color: #444444;
	scrollbar-face-color: #C8E097;
	scrollbar-base-color: #C8E097;
	scrollbar-shadow-color: #C8E097;
	scrollbar-dark-shadow-color:#C8E097;
	scrollbar-highlight-color: #C8E097;
	scrollbar-3dlight-color: #444444;
}

&lt;/style&gt;
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

<xsl:template match="menu/mod/out">
	<xsl:value-of select="$msgs/menu/mod/m[@n='out']"/>
</xsl:template>

<xsl:template match="menu/mod/conf">
	<xsl:value-of select="$msgs/menu/mod/m[@n='conf']"/>
</xsl:template>

<xsl:template match="vqwww/mod/conf">
	<table width="100%">
		<xsl:for-each select="./mod">
			<tr class="conf">
				<td>
					<a class="conf">
						<xsl:attribute name="href">
							<xsl:value-of select="$REQUEST_URI"/>
							<xsl:text disable-output-escaping="yes">?conf=&amp;id=</xsl:text>
							<xsl:value-of select="@id"/>
						</xsl:attribute>

						<xsl:apply-templates select="*[1]"/>
					</a>
				</td>
			</tr>

			<tr class="conf_desc">
				<td>
					<xsl:apply-templates select=".//desc"/>
				</td>
			</tr>
		</xsl:for-each>
	</table>
</xsl:template>

<xsl:include href="file://cpassmod.xslt"/>
<xsl:include href="file://credirmod.xslt"/>
<xsl:include href="file://cautorespmod.xslt"/>
<xsl:include href="file://cloghistmod.xslt"/>

</xsl:stylesheet>
