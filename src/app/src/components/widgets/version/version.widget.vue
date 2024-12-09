<template>
	<Widget :title="title" :footerText="footerText" :actionLabel="actionLabel" noHeader>
		<template v-slot:body>
			<button class="widget-wrapper widget-button flex items-center">
				<i
					class="fas fa-code-branch widget-icon w-16 h-14 !text-3.5xl leading-none box-border p-3 rounded-md mr-4"></i>
				<div class="widget-text flex-col items-start text-left text-gray-400">
					<h3 class="widget-text-header mb-1">Version</h3>
					<p class="widget-state text-xs text-left" id="light">{{ appVersion }}</p>
					<p v-if="isUpdateAvailable" class="text-xs text-red-500">Neue Version verfügbar!</p>
				</div>
			</button>
			<!-- <div v-if="isLoading" class="text-center text-gray-500">Version wird geladen...</div>
			<div v-if="fetchError" class="text-center text-red-500">{{ fetchError }}</div> -->
		</template>
		<template v-slot:footer>
			<span class="text-xs text-gray-500">{{ footerText }}</span>
		</template>
	</Widget>
</template>

<script lang="ts" src="./version.widget.ts"></script>
