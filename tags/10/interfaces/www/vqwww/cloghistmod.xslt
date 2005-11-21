<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="loghist_msgs" select="document(concat('file://cloghistmod.',$LOCALE,'.xml'))/msgs"/>

<xsl:template match="vqwww/menu/mod/loghist">	
	<xsl:value-of select="$loghist_msgs/menu/mod/m[@n='loghist']"/>
</xsl:template>

<xsl:template match="vqwww/mod/loghist/navi">
	<tr class="first" align="center">
		<td colspan="4">
			<xsl:if test="start">
				<a>
					<xsl:attribute name="href">
						<xsl:value-of select="$REQUEST_URI"/>
						<xsl:text>?id=loghist&amp;cnt=</xsl:text>
						<xsl:value-of select="@cnt"/>
						<xsl:text>&amp;start=0</xsl:text>
					</xsl:attribute>
					<xsl:text disable-output-escaping="yes">|&lt;</xsl:text>
				</a>
				<xsl:text disable-output-escaping="yes">&amp;nbsp;</xsl:text>
			</xsl:if>

			<xsl:if test="prev">
				<a>
					<xsl:attribute name="href">
						<xsl:value-of select="$REQUEST_URI"/>
						<xsl:text>?id=loghist&amp;cnt=</xsl:text>
						<xsl:value-of select="@cnt"/>
						<xsl:text>&amp;start=</xsl:text>
						<xsl:value-of select="prev/@start"/>
					</xsl:attribute>
					<xsl:text disable-output-escaping="yes">&lt;&lt;</xsl:text>
				</a>
				<xsl:text disable-output-escaping="yes">&amp;nbsp;</xsl:text>
			</xsl:if>

			<xsl:for-each select="page">
				<xsl:choose>
					<xsl:when test="@cur='1'">
						<xsl:value-of select="@num"/>
					</xsl:when>
					<xsl:otherwise>
						<a>
							<xsl:attribute name="href">
								<xsl:value-of select="$REQUEST_URI"/>
								<xsl:text>?id=loghist&amp;cnt=</xsl:text>
								<xsl:value-of select="parent::navi/@cnt"/>
								<xsl:text>&amp;start=</xsl:text>
								<xsl:value-of select="@start"/>
							</xsl:attribute>
							<xsl:value-of select="@num"/>
						</a>
					</xsl:otherwise>
				</xsl:choose>
				<xsl:text disable-output-escaping="yes">&amp;nbsp;</xsl:text>
			</xsl:for-each>
	
			<xsl:if test="next">
				<a>
					<xsl:attribute name="href">
						<xsl:value-of select="$REQUEST_URI"/>
						<xsl:text>?id=loghist&amp;cnt=</xsl:text>
						<xsl:value-of select="@cnt"/>
						<xsl:text>&amp;start=</xsl:text>
						<xsl:value-of select="next/@start"/>
					</xsl:attribute>
					<xsl:text disable-output-escaping="yes">&gt;&gt;</xsl:text>
				</a>
				<xsl:text disable-output-escaping="yes">&amp;nbsp;</xsl:text>
			</xsl:if>

			<xsl:if test="end">
				<a>
					<xsl:attribute name="href">
						<xsl:value-of select="$REQUEST_URI"/>
						<xsl:text>?id=loghist&amp;cnt=</xsl:text>
						<xsl:value-of select="@cnt"/>
						<xsl:text>&amp;start=</xsl:text>
						<xsl:value-of select="end/@start"/>
					</xsl:attribute>
					<xsl:text disable-output-escaping="yes">&gt;|</xsl:text>
				</a>
			</xsl:if>
		</td>
	</tr>
</xsl:template>

<xsl:template match="vqwww/mod/loghist">
	<xsl:choose>
		<xsl:when test="empty">
			<div class="err">
				<xsl:value-of select="$loghist_msgs/mod/loghist/m[@n='empty']"/>
			</div>
		</xsl:when>
		<xsl:when test="getcant">
			<div class="err">
				<xsl:value-of select="$loghist_msgs/mod/loghist/m[@n='getcant']"/>
			</div>
		</xsl:when>
		<xsl:otherwise>
			<table width="100%">
				<xsl:apply-templates select="navi"/>

				<tr>
					<th>
						<xsl:value-of select="$loghist_msgs/mod/loghist/m[@n='date']"/>
					</th>
					<th>
						<xsl:value-of select="$loghist_msgs/mod/loghist/m[@n='ser']"/>
					</th>
					<th>
						<xsl:value-of select="$loghist_msgs/mod/loghist/m[@n='addr']"/>
					</th>
					<th>
						<xsl:value-of select="$loghist_msgs/mod/loghist/m[@n='res']"/>
					</th>
				</tr>
		
				<xsl:for-each select="login">
					<tr>
						<xsl:if test="not(position() mod 2)">
							<xsl:attribute name="class">sec</xsl:attribute>
						</xsl:if>
						
						<td nowrap="">
							<xsl:value-of select="time"/>
						</td>
						<td nowrap="">
							<xsl:call-template name="ser2txt">
								<xsl:with-param name="ser">
									<xsl:value-of select="ser"/>
								</xsl:with-param>
							</xsl:call-template>
						</td>
						<td nowrap="">
							<xsl:value-of select="ip"/>
						</td>
						<td nowrap="">
							<xsl:call-template name="res2txt">
								<xsl:with-param name="res">
									<xsl:value-of select="res"/>
								</xsl:with-param>
							</xsl:call-template>
						</td>
					</tr>
				</xsl:for-each>
			</table>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Translate result code to description -->
<xsl:template name="res2txt">
	<xsl:param name="res"/>
	<xsl:choose>
		<xsl:when test="$res='0' or $res='2' or $res='3' or $res='5'">
			<xsl:value-of select="$loghist_msgs/mod/loghist/res2txt/m[@n=$res]"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$loghist_msgs/mod/loghist/res2txt/m[@n='other']"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Translate service code to description -->
<xsl:template name="ser2txt">
	<xsl:param name="ser"/>
	<xsl:choose>
		<xsl:when test="$ser='1' or $ser='2' or $ser='3' or $ser='4'">
			<xsl:value-of select="$loghist_msgs/mod/loghist/ser2txt/m[@n=$ser]"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$loghist_msgs/mod/loghist/ser2txt/m[@n='other']"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

</xsl:stylesheet>
