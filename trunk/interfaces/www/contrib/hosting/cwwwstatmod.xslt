<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/menu/mod/wwwstat">Statystyki WWW</xsl:template>

<xsl:template name="wwwstat_doms">
	<xsl:text>Domeny: </xsl:text>
	<xsl:for-each select="dom">
		<a>
			<xsl:attribute name="href">
				<xsl:text>#</xsl:text>
				<xsl:value-of select="@name"/>
			</xsl:attribute>
	
			<xsl:value-of select="@name"/>
		</a>
		
		<xsl:text> </xsl:text>
	</xsl:for-each>
</xsl:template>

<xsl:template match="vqwww/mod/wwwstat">
	<table>
		<xsl:choose>
			<xsl:when test="getcant">
				<tr class="err"><td>Nie można odczytać aktualnych ustawień.</td></tr>
			</xsl:when>
			<xsl:when test="noent">
				<tr class="err"><td>Brak statystyk.</td></tr>
			</xsl:when>
			<xsl:otherwise>
				<tr class="sec">
					<td colspan="2">
						<xsl:call-template name="wwwstat_doms"/>
					</td>
				</tr>
				
				<!-- input form -->
				<tr>
					<th>Adres</th>
				</tr>
				
				<xsl:for-each select="dom">
				    <a>
						<xsl:attribute name="name">
							<xsl:value-of select="@name"/>
						</xsl:attribute>
					</a>
	
				    <xsl:for-each select="./item">
				        <tr>
							<xsl:if test="not(position() mod 2)">
								<xsl:attribute name="class">
									<xsl:text>sec</xsl:text>
								</xsl:attribute>
							</xsl:if>
						
							<td>
					        	<a TARGET="_blank">
									<xsl:attribute name="href">
										<xsl:choose>
											<xsl:when test="$HTTPS">
												<xsl:text>https</xsl:text>
											</xsl:when>
											<xsl:otherwise>
												<xsl:text>http</xsl:text>
											</xsl:otherwise>
										</xsl:choose>
										<xsl:text>://</xsl:text>
										<xsl:value-of select="$SERVER_NAME"/>
										<xsl:text>/log/</xsl:text>
										<xsl:value-of select="ancestor::dom/@name"/>
										<xsl:text>/</xsl:text>
										<xsl:if test="@prefix!=''">
				    	        			<xsl:value-of select="@prefix"/><xsl:text>.</xsl:text>
			        					</xsl:if>
        								<xsl:value-of select="ancestor::dom/@name"/>
										<xsl:text>/</xsl:text>
				        				<xsl:value-of select="ancestor::mod/wwwstat/@sid"/>
	        							<xsl:text>/index.html</xsl:text>
									</xsl:attribute>
				
									<xsl:if test="@prefix!=''">
					            		<xsl:value-of select="@prefix"/><xsl:text>.</xsl:text>
					        		</xsl:if>
					        		<xsl:value-of select="ancestor::dom/@name"/>
					        	</a>
				        	</td>
						</tr>
				    </xsl:for-each>
				</xsl:for-each>
			</xsl:otherwise>
		</xsl:choose>
	</table>
</xsl:template>

</xsl:stylesheet>
