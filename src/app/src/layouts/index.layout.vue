<template>
	<section class="main-section h-full">
		<main class="main flex flex-col grow-[6] shrink-[1] basis-0 justify-between bg-background">
			<div class="main-panel p-3">
				<router-view />
			</div>
			<!--    FOOTER    -->
			<footer class="footer">
				<p class="footer__text">© 2024 Schulz Hygiene GmbH</p>
			</footer>
		</main>
	</section>
</template>

<script src="./index.layout.ts"></script>
